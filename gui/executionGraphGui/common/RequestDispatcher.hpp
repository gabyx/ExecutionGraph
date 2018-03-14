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

#ifndef cefapp_RequestDispatcher_h
#define cefapp_RequestDispatcher_h

#include <executionGraph/common/Assert.hpp>
#include <executionGraph/common/ThreadPool.hpp>
#include <memory>
#include <unordered_set>
#include <vector>
#include "common/Loggers.hpp"
#include "common/Request.hpp"
#include "common/Response.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    A message dispatcher which dispatches to shared message handlers.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
template<typename THandlerType, typename TRequestType, typename TResponseType>
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
    void addRequest(Request&& request, Response&& response);

public:
    //! Adds a message handler `handler` for an optional specific request id `requestType`.
    //! Messages are first dispatched to all specific handlers added with a `requestType`.
    bool addHandler(std::shared_ptr<HandlerType> handler, const std::string& requestType)
    {
        std::scoped_lock<std::mutex> lock(m_access);

        if(!handler || requestType.empty())
        {
            EXECGRAPH_ASSERT(false, "nullptr or empty requestType")
            return false;
        }
        Id id = handler->getId();

        EXECGRAPH_THROW_EXCEPTION_IF(m_handlerStorage.find(id) != m_handlerStorage.end(),
                                     "MessageHandler with id: " << id.getUniqueName() << " already exists!");

        m_handlerStorage.emplace(id, HandlerData{requestType, handler.get()});
        m_specificHandlers[requestType].emplace(handler.get());
        return true;
    }

    //! Adds a message handler `handler` to the back or the front of all general handlers.
    template<bool insertAtFront = false>
    bool addHandler(std::shared_ptr<HandlerType> handler)
    {
        std::scoped_lock<std::mutex> lock(m_access);

        if(!handler)
        {
            EXECGRAPH_ASSERT(false, "nullptr!")
            return false;
        }

        Id id = handler->getId();

        EXECGRAPH_THROW_EXCEPTION_IF(m_handlerStorage.find(id) != m_handlerStorage.end(),
                                     "MessageHandler with id: " << id.getUniqueName() << " already exists!");

        if(insertAtFront)
        {
            m_handlerStorage.emplace(id, HandlerData{0, handler});
            m_generalHandlers.emplace_back(handler.get());
        }
        else
        {
            m_handlerStorage.emplace(id, HandlerData{m_generalHandlers.size(), handler});
            m_generalHandlers.emplace_back(handler.get());
        }
        return true;
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
            handler = it->second.m_handler;
            if(it->second.m_requestType.empty())
            {
                EXECGRAPH_ASSERT(it->second.m_index < m_generalHandlers.size(), "Wrong index!")
                m_generalHandlers.erase(m_generalHandlers.begin() + it->index);  // Remove from list
            }
            else
            {
                m_specificHandlers[it->second.m_requestType].erase(it->second.m_handler.get());
            }

            m_handlerStorage.erase(it);  // Remove from storage
        }
        return handler;
    }

    //! Starts the dispatcher thread.
    void start()
    {
        m_pool.start();
    }

    //! Stops the dispatcher thread.
    void stop()
    {
        m_pool.join();
    }

private:
    struct HandlerData
    {
        HandlerData(const std::string& requestType, std::shared_ptr<HandlerType> handler)
            : m_requestType(requestType), m_handler(handler)
        {}

        HandlerData(const std::size_t& index, std::shared_ptr<HandlerType> handler)
            : m_index(index), m_handler(handler)
        {}

        const std::string m_requestType;               //! The requestType if it is a specific handler.
        const std::size_t m_index = 0;                 //! Index into `m_generalHandlers` if `m_requestType` is empty.
        const std::shared_ptr<HandlerType> m_handler;  //! The message handler.
    };

private:
    std::unordered_map<std::string, std::unordered_set<HandlerType*>> m_specificHandlers;  //! Handlers for a specific request id (handled first).
    std::vector<HandlerType*> m_generalHandlers;                                           //!< The handlers to which all messages are dispatched (no requestType).
    std::unordered_map<typename HandlerType::Id, HandlerData> m_handlerStorage;            //!< Storage for handlers.
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
            std::scoped_lock<std::mutex> lock(m_d.m_access);

            const std::string requestType = m_request.getRequestType();

            // Check all specific handlers first
            auto it = m_d.m_specificHandlers.find(requestType);
            if(it != m_d.m_specificHandlers.end())
            {
                for(auto* handler : it->second)
                {
                    if(handler->handleRequest(m_request, m_response))
                    {
                        return;
                    }
                }
            }

            // Check all general handlers
            for(auto* handler : m_d.m_generalHandlers)
            {
                if(handler->handleRequest(m_request, m_response))
                {
                    return;
                }
            }

            EXECGRAPHGUI_BACKENDLOG_WARN(
                "RequestDispatcher: Request id: '{0}' has not "
                "been handled, it will be cancled!",
                m_request.getId().getUniqueName());
            EXECGRAPH_THROW_EXCEPTION("No handler found!");
        };

        void onTaskException(std::exception_ptr e)
        {
            EXECGRAPHGUI_BACKENDLOG_WARN(
                "RequestDispatcher: Request id: '{0}' has thrown exception, "
                "it will be cancled!",
                m_request.getId().getUniqueName());
            m_response.setCanceled(e);
        };

    private:
        RequestDispatcher& m_d;   //!< Dispatcher.
        RequestType m_request;    //!< The request to handle.
        ResponseType m_response;  //!< The response to handle.
    };

private:
    friend class TaskHandleRequest;

private:
    executionGraph::ThreadPool<TaskHandleRequest> m_pool{1};  //! One seperate thread will handle all messages for this dispatcher.
};

template<typename THandlerType, typename TRequestType, typename TResponseType>
template<typename Request, typename Response>
void RequestDispatcher<THandlerType, TRequestType, TResponseType>::addRequest(Request&& request, Response&& response)
{
    m_pool.getQueue()->emplace(*this, std::forward<Request>(request), std::forward<Response>(response));
}

#endif
