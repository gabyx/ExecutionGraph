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

#include <executionGraph/common/Asserts.hpp>
#include <memory>
#include <unordered_set>
#include <vector>
#include <wrapper/cef_message_router.h>

//! A message dispatcher which dispatches to shared message handlers.
template<typename THandlerType>
class MessageDispatcher final : public CefMessageRouterBrowserSide::Handler
{
public:
    using HandlerType = THandlerType;
    using Id          = typename HandlerType::Id;

public:
    MessageDispatcher()          = default;
    virtual ~MessageDispatcher() = default;

public:
    bool OnQuery(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int64 queryId,
                 const CefString& request,
                 bool persistent,
                 CefRefPtr<Callback> callback) override
    {
        // Check first all specific handlers
        // todo ....
        // if message handler handled the message -> return

        // otherwise, dispatch to all other handlers
        for(auto* handler : m_handlers)
        {
            if(handler->OnQuery(browser, frame, queryId, request, persistent, callback))
            {
                return true;  // it was handled!
            }
        }
        return false;
    }

    //! Callback for binary data from XHR Requests
    bool HandleMessage()
    {
        // Check first all specific handlers
        // todo ...
        // if message handler handled the message -> return

        // otherwise, dispatch to all other handlers
        // todo ...
        return false;
    }

public:
    //! Sets a message handler `handler` for the specific request id `requestId`.
    void SetHandler(const std::string& requestId, std::shared_ptr<HandlerType> handler)
    {
        if(!handler || requestId.empty())
        {
            EXECGRAPH_ASSERT(false, "nullptr or empty requestId")
            return;  // nullptr
        }
        Id id = handler->getId();

        EXECGRAPH_THROW_EXCEPTION_IF(m_handlerStorage.find(id) != m_handlerStorage.end(),
                                     "MessageHandler with id: " << id.getFullName() << " already exists!");

        m_handlerStorage.emplace(id, HandlerData{requestId, handler.get()});
        m_specificHandlers.emplace(requestId, handler.get());
    }

    //! Adds a message handler `handler` to the back of the dispatch sequence.
    template<bool insertAtFront = false>
    void AddHandler(std::shared_ptr<HandlerType> handler)
    {
        if(!handler)
        {
            EXECGRAPH_ASSERT(false, "nullptr!")
            return;  // nullptr
        }

        Id id = handler->getId();

        EXECGRAPH_THROW_EXCEPTION_IF(m_handlerStorage.find(id) != m_handlerStorage.end(),
                                     "MessageHandler with id: " << id.getFullName() << " already exists!");

        if(insertAtFront)
        {
            m_handlerStorage.emplace(id, HandlerData{0, handler});
            m_handlers.emplace_back(handler.get());
        }
        else
        {
            m_handlerStorage.emplace(id, HandlerData{m_handlers.size(), handler});
            m_handlers.emplace_back(handler.get());
        }
    }

    //! Removes a message handler with id `id`.
    std::shared_ptr<HandlerType> RemoveHandler(Id id)
    {
        std::shared_ptr<HandlerType> handler;

        auto it = m_handlerStorage.find(id);
        if(it != m_handlerStorage.end())
        {
            handler = it->second.m_handler;
            if(it->second.m_requestId.empty())
            {
                EXECGRAPH_ASSERT(it->second.m_index < m_handlers.size(), "Wrong index!")
                m_handlers.erase(m_handlers.begin() + it->index);  // Remove from list
            }
            else
            {
                m_specificHandlers.erase(it->second.m_requestId);
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
        const std::size_t m_index = 0;                 //! Index into `m_handlers` if `m_requestId` is empty.
        const std::shared_ptr<HandlerType> m_handler;  //! The message handler.
    };

private:
    std::unordered_map<std::string, HandlerType*> m_specificHandlers;            //! A unique handler for each specific request id.
    std::vector<HandlerType*> m_handlers;                                        //!< The handlers to which all messages are dispatched (no requestId).
    std::unordered_map<typename HandlerType::Id, HandlerData> m_handlerStorage;  //!< Storage for handlers.
};

#endif