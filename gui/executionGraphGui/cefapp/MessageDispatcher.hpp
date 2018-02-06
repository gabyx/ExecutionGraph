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
    MessageDispatcher()  = default;
    ~MessageDispatcher() = default;

public:
    bool OnQuery(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int64 queryId,
                 const CefString& request,
                 bool persistent,
                 CefRefPtr<Callback> callback) override
    {
        // Dispatch to all handlers
        for(auto* handler : m_handlers)
        {
            if(handler->OnQuery(browser, frame, queryId, request, persistent, callback))
            {
                return true;  // it was handled!
            }
        }
        return false;
    }

public:
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

        if(m_handlerMap.find(handler->getId()) != m_handlerMap.end())
        {
            EXECGRAPH_ASSERT(false, "MessageHandler with id: " << handler->getId().getFullName() << " already exists!");
            return;
        }
        if(insertAtFront)
        {
            m_handlerMap.emplace(id, HandlerData{0, handler});
            m_handlers.emplace_back(handler.get());
        }
        else
        {
            m_handlerMap.emplace(id, HandlerData{m_handlers.size(), handler});
            m_handlers.emplace_back(handler.get());
        }
    }

    //! Removes a message handler from this dispatched with id `id`.
    std::shared_ptr<HandlerType> RemoveHandler(Id id)
    {
        std::shared_ptr<HandlerType> handler;

        auto it = m_handlerMap.find(id);
        if(it != m_handlerMap.end())
        {
            handler = *it;
            EXECGRAPH_ASSERT(it->index < m_handlers.size(), "Wrong index!")
            m_handlers.erase(m_handlers.begin() + it->index);  // Remove from list
            m_handlerMap.erase(it);                            // Remove from map
        }
        return handler;
    }

private:
    struct HandlerData
    {
        std::size_t m_index;                     //! Index into `m_handlers`.
        std::shared_ptr<HandlerType> m_handler;  //! The message handler
    };

private:
    std::vector<HandlerType*> m_handlers;                                    //!< The handlers to which all messages are dispatched.
    std::unordered_map<typename HandlerType::Id, HandlerData> m_handlerMap;  //!< Storage for handlers.
};

#endif