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
#include "executionGraphGui/common/Request.hpp"
#include "executionGraphGui/common/Response.hpp"

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
         bool bUseThreadsForDispatch = true,
         bool bDoFowardRequest       = false>
class RequestDispatcher
{
public:
    using HandlerType  = THandlerType;
    using RequestType  = TRequestType;
    using ResponseType = TResponseType;

    using Id = typename HandlerType::Id;

public:
    RequestDispatcher() = default;

    //! Destructor automatically stops the pool by its DTOR.
    virtual ~RequestDispatcher() = default;

public:
    //! Handle a general request/response. Can be called on any thread!
    template<typename Request, typename Response>
    void handleRequest(Request&& request, Response&& response)
    {
        if(bUseThreadsForDispatch)
        {
            // Run the dispatch in
            m_pool.getQueue()->emplace(*this, std::forward<Request>(request), std::forward<Response>(response));
        }
        else
        {
            // Run the task in this thread
            Pool::Consumer::Run(
                Task(*this, std::forward<Request>(request), std::forward<Response>(response)),
                std::this_thread::get_id());
        }
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
        if(bUseThreadsForDispatch)
        {
            m_pool.start();
        }
    }

    //! Stops the dispatcher thread.
    void stop()
    {
        if(bUseThreadsForDispatch)
        {
            m_pool.join();
        }
    }

private:
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
    class TaskHandleRequest
    {
    public:
        template<typename Request, typename Response>
        TaskHandleRequest(RequestDispatcher& d,
                          Request&& request,
                          Response&& response)
            : m_d(d)
            , m_request(std::forward<Request>(request))
            , m_response(std::forward<Response>(response))
        {
        }

        TaskHandleRequest(TaskHandleRequest&&) = default;
        TaskHandleRequest& operator=(TaskHandleRequest&&) = default;

        void runTask(std::thread::id threadId)
        {
            std::shared_ptr<HandlerType> handler;
            std::string requestType;
            {  // Lock start
                std::scoped_lock<std::mutex> lock(m_d.m_access);
                // Get the request type
                requestType = m_request->getURL().string();

                // Find handler
                auto it = m_d.m_specificHandlers.find(requestType);

                if(it != m_d.m_specificHandlers.end())
                {
                    handler = it->second->m_handler;
                }
            }  // Lock end

            // Handling the request
            if(handler)
            {
                handler->handleRequest(*m_request, *m_response);
                if(!m_response->isResolved())
                {
                    EXECGRAPHGUI_BACKENDLOG_WARN("RequestDispatcher: Request id: '{0}' (url: '{1}') has not been handled correctly, it will be cancled!",
                                                 m_request->getId().toString(),
                                                 requestType);
                }
            }
            else
            {
                EXECGRAPHGUI_BACKENDLOG_WARN("RequestDispatcher: No handler found for request id: '{0}' (url: '{1}')!, it will be cancled!",
                                             m_request->getId().toString(),
                                             requestType);
            }
        };

        void onTaskException(std::exception_ptr e)
        {
            EXECGRAPHGUI_BACKENDLOG_WARN("RequestDispatcher: Request id: '{0}' (url: '{1}') has thrown exception, it will be cancled!",
                                         m_request->getId().toString(),
                                         m_request->getURL().string());
            m_response->setCanceled(e);
        };

    private:
        RequestDispatcher& m_d;                    //!< Dispatcher.
        std::unique_ptr<RequestType> m_request;    //!< The request to handle.
        std::unique_ptr<ResponseType> m_response;  //!< The response to handle.
    };

    class TaskForwardRequest
    {
    public:
        template<typename Request, typename Response>
        TaskForwardRequest(RequestDispatcher& d,
                           Request&& request,
                           Response&& response)
            : m_d(d)
            , m_request(std::forward<Request>(request))
            , m_response(std::forward<Response>(response))
        {
        }

        TaskForwardRequest(TaskForwardRequest&&) = default;
        TaskForwardRequest& operator=(TaskForwardRequest&&) = default;

        void runTask(std::thread::id threadId)
        {
            std::shared_ptr<HandlerType> handler;

            {  // Lock start
                std::scoped_lock<std::mutex> lock(m_d.m_access);
                // Get the request type
                const std::string requestType = m_request->getURL().string();

                // Find handler
                auto it = m_d.m_specificHandlers.find(requestType);

                if(it != m_d.m_specificHandlers.end())
                {
                    handler = it->m_handler;
                }
            }  // Lock end

            // Handling the request
            if(handler)
            {
                handler->handleRequest(std::move(m_request), std::move(m_response));
            }
            else
            {
                EXECGRAPHGUI_BACKENDLOG_WARN("RequestDispatcher: No handler found for request id: '{0}'!, it will be cancled!",
                                             m_request->getId().toString());
            }
        }

        void onTaskException(std::exception_ptr e)
        {
            EXECGRAPHGUI_BACKENDLOG_WARN("RequestDispatcher: Forwarding request has thrown exception!");
        };

    private:
        RequestDispatcher& m_d;                    //!< Dispatcher.
        std::unique_ptr<RequestType> m_request;    //!< The request to handle.
        std::unique_ptr<ResponseType> m_response;  //!< The response to handle.
    };

private:
    friend class TaskHandleRequest;
    friend class TaskForwardRequest;

    using Task = meta::if_<meta::bool_<bDoFowardRequest>, TaskForwardRequest, TaskHandleRequest>;

private:
    using Pool = executionGraph::ThreadPool<Task>;
    Pool m_pool{1};  //! One seperate thread will handle all messages for this dispatcher.
};

#endif
