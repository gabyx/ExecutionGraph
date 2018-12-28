//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Tue Feb 06 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraphGui_common_RequestDispatcher_hpp
#define executionGraphGui_common_RequestDispatcher_hpp

#include <memory>
#include <unordered_set>
#include <vector>
#include <meta/meta.hpp>
#include <executionGraph/common/Assert.hpp>
#include <executionGraph/common/ThreadPool.hpp>
#include "executionGraphGui/common/Exception.hpp"
#include "executionGraphGui/common/Loggers.hpp"

namespace details
{
    template<typename Request,
             typename Response,
             typename Handler,
             bool doForwarding>
    struct TaskHandleRequest
    {
        template<typename TRequest, typename TResponse>
        TaskHandleRequest(std::shared_ptr<Handler> handler,
                          TRequest&& request,
                          TResponse&& response)
            : m_handler(handler)
            , m_request(std::forward<TRequest>(request))
            , m_response(std::forward<TResponse>(response))
        {
        }

        TaskHandleRequest(TaskHandleRequest&&) = default;

        void runTask(std::thread::id threadId)
        {
            if constexpr(!doForwarding)
            {
                m_handler->handleRequest(m_request, m_response);
                if(!m_response.isResolved())
                {
                    EXECGRAPHGUI_THROW(
                        "RequestDispatcher: Request id: '{0}' (url: '{1}') "
                        "has not been handled correctly, it will be cancled!",
                        m_request.getId().toString(),
                        m_request.getTarget());
                }
            }
            else
            {
                // Forward: Move the request and response into the handler.
                m_handler->handleRequest(std::move(m_request), std::move(m_response));
            }
        };

        void onTaskException(std::exception_ptr e)
        {
            EXECGRAPHGUI_BACKENDLOG_WARN(
                "RequestDispatcher: Cancel request id: '{0}' [url: '{1}']",
                m_request.getId().toString(),
                m_request.getTarget());
            m_response.setCanceled(e);
        };

    private:
        std::shared_ptr<Handler> m_handler;  //!< Dispatcher.
        Request m_request;                   //!< The request to handle.
        Response m_response;                 //!< The response to handle.
    };
}  // namespace details

/* ---------------------------------------------------------------------------------------*/
/*!
    A message dispatcher which dispatches to shared message handlers.
    Forwarding the message can be achieved where the message handlers accept a moved
    response and promise.
    
    Important:
    The THandler::handleRequest function needs to be thread-safe because it can be called
    as it will be called on the same instance on possible multiple threads.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
template<typename THandler,
         typename TRequest,
         typename TResponse,
         bool useThreadsForDispatch = true,
         bool doForwardRequest      = false>
class RequestDispatcher
{
public:
    using Handler  = THandler;
    using Request  = TRequest;
    using Response = TResponse;

    static_assert(std::is_move_constructible_v<Request>,
                  "Request needs to be movable constructible into the task");
    static_assert(std::is_move_constructible_v<Response>,
                  "Response needs to be movable constructible into the task");

    using Id = typename Handler::Id;

public:
    RequestDispatcher() = default;

    //! Destructor automatically stops the pool by its DTOR.
    virtual ~RequestDispatcher() = default;

public:
    //! Handle a general request/response [thread-safe]
    //! Request/Response are moved (not forwarded) if a handler is found
    //! and `useThreadsForDispatch` is used.
    //! @return `rrue` if it gets handled (what ever outcome).
    template<typename TTRequest, typename TTResponse>
    bool handleRequest(TTRequest&& request, TTResponse&& response)
    {
        auto handler = matchHandler(request);

        if(handler)
        {
            if constexpr(useThreadsForDispatch)
            {
                // Run the handler in the thread pool.
                m_pool.getQueue()->emplace(handler,
                                           std::move(request),
                                           std::move(response));
            }
            else
            {
                // Run the task in this thread.
                Pool::Consumer::Run(
                    Task<false>{handler, request, response},
                    std::this_thread::get_id());
            }
        }

        return handler != nullptr;
    }

public:
    //! Adds a message handler `handler` for specific request types `handler.getRequests()`.
    void addHandler(std::shared_ptr<Handler> handler)
    {
        std::scoped_lock<std::mutex> lock(m_access);

        const auto& requestTypes = handler->getRequestTypes();
        EXECGRAPHGUI_THROW_IF(!handler || requestTypes.size() == 0, "nullptr or no Requests");

        const Id& id = handler->getId();
        EXECGRAPHGUI_THROW_IF(m_handlerStorage.find(id) != m_handlerStorage.end(),
                              "MessageHandler with id: '{0}' already exists!",
                              id.toString());

        auto p = m_handlerStorage.emplace(id, HandlerData{std::move(requestTypes), handler});

        for(auto& target : p.first->second.m_targets)
        {
            EXECGRAPHGUI_THROW_IF(m_specificHandlers.find(target) != m_specificHandlers.end(),
                                  "Handler for request target: '{0}' already registered!",
                                  target);
            m_specificHandlers[target] = &(p.first->second);
        }
    }

    //! Removes a message handler with id `id`.
    //! @return the removed handler.
    std::shared_ptr<Handler> removeHandler(Id id)
    {
        std::scoped_lock<std::mutex> lock(m_access);

        std::shared_ptr<Handler> handler;

        auto it = m_handlerStorage.find(id);
        if(it != m_handlerStorage.end())
        {
            for(auto& target : it->second.m_targets)
            {
                m_specificHandlers.erase(target);
            }
            handler = it->second.m_handler;
            m_handlerStorage.erase(it);  // Remove from storage
        }
        return handler;
    }

    //! Starts the dispatcher thread.
    void start()
    {
        if constexpr(useThreadsForDispatch)
        {
            m_pool.start();
        }
    }

    //! Stops the dispatcher thread.
    void stop()
    {
        if constexpr(useThreadsForDispatch)
        {
            m_pool.join();
        }
    }

private:
    //! Get the first handler which matches the request.
    std::shared_ptr<Handler> matchHandler(const Request& request)
    {
        // Lock start
        std::scoped_lock<std::mutex> lock(m_access);
        // Get the request type
        auto Request = request.getTarget().string();

        // Find handler
        auto it = m_specificHandlers.find(Request);

        if(it != m_specificHandlers.end())
        {
            return it->second->m_handler;
        }
        return nullptr;
    }

    struct HandlerData
    {
        HandlerData(const std::unordered_set<std::string>& requestTargets,
                    std::shared_ptr<Handler> handler)
            : m_targets(requestTargets), m_handler(handler)
        {}

        const std::unordered_set<std::string> m_targets;  //!< The request if it is a specific handler.
        const std::shared_ptr<Handler> m_handler;         //!< The message handler.
    };

private:
    std::unordered_map<std::string, HandlerData*> m_specificHandlers;        //!< Handlers for a specific request type (handled first).
    std::unordered_map<typename Handler::Id, HandlerData> m_handlerStorage;  //!< Storage for handlers.
    std::mutex m_access;

    template<bool useValueSemantics = true>
    using Task = meta::if_<meta::bool_<useValueSemantics>,
                           details::TaskHandleRequest<Request, Response, Handler, doForwardRequest>,
                           details::TaskHandleRequest<Request&, Response&, Handler, doForwardRequest>>;

private:
    struct NoPool
    {
        NoPool(std::size_t) {}
        using Consumer = typename executionGraph::ThreadPool<Task<false>>::Consumer;
    };
    using Pool = meta::if_<meta::bool_<useThreadsForDispatch>,
                           executionGraph::ThreadPool<Task<true>>,
                           NoPool>;
    Pool m_pool{1};  //! One seperate thread will handle all messages for this dispatcher.
};

#endif
