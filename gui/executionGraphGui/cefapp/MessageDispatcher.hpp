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

#ifndef backend_File_Scheme_Handler_Factory_h
#define backend_File_Scheme_Handler_Factory_h

#include <executionGraph/common/Assert.hpp>
#include <executionGraph/common/ThreadPool.hpp>
#include <memory>
#include <unordered_set>
#include <vector>
#include <wrapper/cef_message_router.h>
#include "cefapp/Loggers.hpp"
#include "cefapp/Request.hpp"
#include "cefapp/Response.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    A message dispatcher which dispatches to shared message handlers.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
template<typename THandlerType>
class MessageDispatcher : public CefMessageRouterBrowserSide::Handler
{
public:
    using HandlerType = THandlerType;
    using Id          = typename HandlerType::Id;

public:
    MessageDispatcher()          = default;
    virtual ~MessageDispatcher() = default;

private:
    bool OnQuery(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int64 queryId,
                 const CefString& request,
                 bool persistent,
                 CefRefPtr<Callback> callback) override
    {
        CEF_REQUIRE_UI_THREAD();
        //todo wrap here
        return true;
    }

public:
    //! Handle a general request/response. Can be called on any thread!
    template<typename Request, typename Response>
    void AddRequest(Request&& request, Response&& response)
    {
        m_pool.getQueue()->emplace(TaskHandleRequest{this,
                                                     std::forward<Request>(request),
                                                     std::forward<Response>(response)});
    }

public:
    //! Adds a message handler `handler` for an optional specific request id `requestId`.
    //! Messages are first dispatched to all specific handlers added with a `requestId`.
    bool AddHandler(std::shared_ptr<HandlerType> handler, const std::string& requestId)
    {
        std::scoped_lock<std::mutex> lock(m_access);

        if(!handler || requestId.empty())
        {
            EXECGRAPH_ASSERT(false, "nullptr or empty requestId")
            return false;
        }
        Id id = handler->getId();

        EXECGRAPH_THROW_EXCEPTION_IF(m_handlerStorage.find(id) != m_handlerStorage.end(),
                                     "MessageHandler with id: " << id.getUniqueName() << " already exists!");

        m_handlerStorage.emplace(id, HandlerData{requestId, handler.get()});
        m_specificHandlers[requestId].emplace(handler.get());
        return true;
    }

    //! Adds a message handler `handler` to the back or the front of all general handlers.
    template<bool insertAtFront = false>
    bool AddHandler(std::shared_ptr<HandlerType> handler)
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
    std::shared_ptr<HandlerType> RemoveHandler(Id id)
    {
        std::scoped_lock<std::mutex> lock(m_access);

        std::shared_ptr<HandlerType> handler;

        auto it = m_handlerStorage.find(id);
        if(it != m_handlerStorage.end())
        {
            handler = it->second.m_handler;
            if(it->second.m_requestId.empty())
            {
                EXECGRAPH_ASSERT(it->second.m_index < m_generalHandlers.size(), "Wrong index!")
                m_generalHandlers.erase(m_generalHandlers.begin() + it->index);  // Remove from list
            }
            else
            {
                m_specificHandlers[it->second.m_requestId].erase(it->second.m_handler.get());
            }

            m_handlerStorage.erase(it);  // Remove from storage
        }
        return handler;
    }

private:
    struct HandlerData
    {
        HandlerData(const std::string& requestId, std::shared_ptr<HandlerType> handler)
            : m_requestId(requestId), m_handler(handler)
        {}

        HandlerData(const std::size_t& index, std::shared_ptr<HandlerType> handler)
            : m_index(index), m_handler(handler)
        {}

        const std::string m_requestId;                 //! The requestId if it is a specific handler.
        const std::size_t m_index = 0;                 //! Index into `m_generalHandlers` if `m_requestId` is empty.
        const std::shared_ptr<HandlerType> m_handler;  //! The message handler.
    };

private:
    std::unordered_map<std::string, std::unordered_set<HandlerType*>> m_specificHandlers;  //! Handlers for a specific request id (handled first).
    std::vector<HandlerType*> m_generalHandlers;                                           //!< The handlers to which all messages are dispatched (no requestId).
    std::unordered_map<typename HandlerType::Id, HandlerData> m_handlerStorage;            //!< Storage for handlers.
    std::mutex m_access;

private:
    class TaskHandleRequest
    {
    public:
        template<typename Request, typename Response>
        TaskHandleRequest(MessageDispatcher& d,
                          Request&& request,
                          Response&& response)
            : m_d(d)
            , m_request(std::forward<Request>(request))
            , m_response(std::forward<Response>(response))
        {
            EXECGRAPH_ASSERT(m_request, "Message is nullptr!");
        }

        void runTask(std::thread::id threadId)
        {
            std::scoped_lock<std::mutex> lock(m_d.m_access);

            const std::string requestId = m_request->getRequestId();

            // Check all specific handlers first
            auto it = m_d.m_specificHandlers.find(requestId);
            if(it != m_d.m_specificHandlers.end())
            {
                for(auto* handler : it->second)
                {
                    if(handler->handleRequest(*m_request, *m_response))
                    {
                        return;
                    }
                }
            }

            // Check all general handlers
            for(auto* handler : m_d.m_generalHandlers)
            {
                if(handler->handleRequest(*m_request, *m_response))
                {
                    return;
                }
            }

            EXECGRAPHGUI_APPLOG_WARN("Request with requestId: '{0}' has not been handled, it will be cancled!", requestId);
            m_response->setCanceled("No handler found!");
        };

        void onTaskException(const std::string& what)
        {
            EXECGRAPHGUI_APPLOG_WARN("Request with requestId: '{0}' has thrown exception: {1}, it will be cancled!", m_request->getRequestId(), what);
            m_response->setCanceled(what);
        };

    private:
        std::unique_ptr<Request> m_request;           //!< The request to handle.
        std::unique_ptr<ResponsePromise> m_response;  //!< The response to handle.
        MessageDispatcher& m_d;                       //!< Dispatcher.
    };

private:
    friend class TaskHandleRequest;

private:
    executionGraph::ThreadPool<TaskHandleRequest> m_pool{1};  //! One seperate thread will handle all messages for this dispatcher.
};

#endif