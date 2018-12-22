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

/* ---------------------------------------------------------------------------------------*/
/*!
    A message dispatcher which dispatches to shared message handlers.
    Forwarding the message can be achieved where the message handlers accept a moved
    response and promise.
    
    Important:
    The THandlerType::handleRequest function needs to be thread-safe (stateless handler) 
    as it will be called on the same instance on possible multiple threads.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
template<typename THandlerType,
         typename TRequestType,
         typename TResponseType,
         bool useThreadsForDispatch = true,
         bool doForwardRequest      = false>
class RequestDispatcher
{
public:
    using HandlerType  = THandlerType;
    using RequestType  = TRequestType;
    using ResponseType = TResponseType;

    static_assert(std::is_move_constructible_v<RequestType>,
                  "Request needs to be movable constructible into the task");
    static_assert(std::is_move_constructible_v<ResponseType>,
                  "Response needs to be movable constructible into the task");

    using Id = typename HandlerType::Id;

public:
    RequestDispatcher() = default;

    //! Destructor automatically stops the pool by its DTOR.
    virtual ~RequestDispatcher() = default;

public:
    //! Handle a general request/response.
    //! Can be called on any thread!
    //! Request/Response are always moved(not forwarded) if a handler is found
    //! in which case the result is `true`.
    template<typename Request, typename Response>
    bool handleRequest(Request&& request, Response&& response)
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
                // here no move into the Task is necessary!
                Pool::Consumer::Run(
                    Task{handler,
                         std::move(request),
                         std::move(response)},
                    std::this_thread::get_id());
            }
        }

        return handler != nullptr;
    }

public:
    //! Adds a message handler `handler` for specific request types `handler.getRequestTypes()`.
    void addHandler(std::shared_ptr<HandlerType> handler)
    {
        std::scoped_lock<std::mutex> lock(m_access);

        const auto& requestTypes = handler->getRequestTypes();
        EXECGRAPHGUI_THROW_IF(!handler || requestTypes.size() == 0, "nullptr or no requestTypes");

        const Id& id = handler->getId();
        EXECGRAPHGUI_THROW_IF(m_handlerStorage.find(id) != m_handlerStorage.end(),
                              "MessageHandler with id: '{0}' already exists!",
                              id.toString());

        auto p = m_handlerStorage.emplace(id, HandlerData{requestTypes, handler});

        for(auto& requestType : requestTypes)
        {
            EXECGRAPHGUI_THROW_IF(m_specificHandlers.find(requestType) != m_specificHandlers.end(),
                                  "Handler for request type: '{0}' already registered!",
                                  requestType);
            m_specificHandlers[requestType] = &(p.first->second);
        }
    }

    //! Removes a message handler with id `id`.
    //! @return the removed handler.
    std::shared_ptr<HandlerType> removeHandler(Id id)
    {
        std::scoped_lock<std::mutex> lock(m_access);

        std::shared_ptr<HandlerType> handler;

        auto it = m_handlerStorage.find(id);
        if(it != m_handlerStorage.end())
        {
            for(auto& requestType : it->second.m_requestTypes)
            {
                m_specificHandlers.erase(requestType);
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
    std::shared_ptr<HandlerType> matchHandler(const RequestType& request)
    {
        // Lock start
        std::scoped_lock<std::mutex> lock(m_access);
        // Get the request type
        auto requestType = request.getTarget().string();

        // Find handler
        auto it = m_specificHandlers.find(requestType);

        if(it != m_specificHandlers.end())
        {
            return it->second->m_handler;
        }
        return nullptr;
    }

    struct HandlerData
    {
        HandlerData(const std::unordered_set<std::string>& requestTypes, std::shared_ptr<HandlerType> handler)
            : m_requestTypes(requestTypes), m_handler(handler)
        {}

        const std::unordered_set<std::string> m_requestTypes;  //!< The requestType if it is a specific handler.
        const std::shared_ptr<HandlerType> m_handler;          //!< The message handler.
    };

private:
    std::unordered_map<std::string, HandlerData*> m_specificHandlers;            //!< Handlers for a specific request type (handled first).
    std::unordered_map<typename HandlerType::Id, HandlerData> m_handlerStorage;  //!< Storage for handlers.
    std::mutex m_access;

private:
    template<bool doForwarding>
    struct TaskHandleRequest
    {
        TaskHandleRequest(std::shared_ptr<HandlerType> handler,
                          RequestType&& request,
                          ResponseType&& response)
            : m_handler(handler)
            , m_request(std::move(request))
            , m_response(std::move(response))
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
                m_handler->handleRequest(std::move(m_request), std::move(m_response));
            }
        };

        void onTaskException(std::exception_ptr e)
        {
            EXECGRAPHGUI_BACKENDLOG_WARN(
                "RequestDispatcher: Request id: '{0}' (url: '{1}') "
                "has thrown exception, it will be cancled!",
                m_request.getId().toString(),
                m_request.getTarget());
            m_response.setCanceled(e);
        };

    private:
        std::shared_ptr<HandlerType> m_handler;  //!< Dispatcher.
        RequestType m_request;                   //!< The request to handle.
        ResponseType m_response;                 //!< The response to handle.
    };

private:
    using Task = TaskHandleRequest<doForwardRequest>;

private:
    struct NoPool
    {
        NoPool(std::size_t) {}
        using Consumer = typename executionGraph::ThreadPool<Task>::Consumer;
    };
    using Pool = std::conditional_t<useThreadsForDispatch, executionGraph::ThreadPool<Task>, NoPool>;
    Pool m_pool{1};  //! One seperate thread will handle all messages for this dispatcher.
};

#endif
