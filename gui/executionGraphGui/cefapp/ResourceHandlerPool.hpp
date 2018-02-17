//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Sat Feb 17 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef cefapp_ResourceHandlerPool_h
#define cefapp_ResourceHandlerPool_h

#include <algorithm>
#include <cef_base.h>
#include <cefapp/Loggers.hpp>
#include <deque>
#include <executionGraph/common/Assert.hpp>
#include <executionGraph/common/Identifier.hpp>
#include <unordered_map>
#include <wrapper/cef_helpers.h>

//! Some stupid pool for resource handlers.
template<typename THandlerType>
class ResourceHandlerPool final
{
public:
    using HandlerType                      = THandlerType;
    using PointerType                      = CefRefPtr<HandlerType>;
    using CallbackOnRequestHandlerFinished = void (*)(PointerType);
    using Id                               = executionGraph::Id;

public:
    ResourceHandlerPool() { batchAllocateHandlers<true>(); }
    ~ResourceHandlerPool() = default;

    PointerType checkoutUnusuedHandler();

private:
    std::deque<PointerType> m_unusedHandlers;                //!< A FIFO queue of unused request handlers.
    std::unordered_map<Id, PointerType> m_resourceHandlers;  //!< A pool of request handlers.

private:
    void onRequestHandlerFinished(const Id& id);

private:
    static const unsigned int c_batchHandlerAllocSize = 5;
    template<bool inCtor = false>
    void batchAllocateHandlers(unsigned int size = c_batchHandlerAllocSize);
};

//! Allocate some handlers.
template<typename HandlerType>
template<bool inCtor>
void ResourceHandlerPool<HandlerType>::batchAllocateHandlers(unsigned int size)
{
    // Only check thread if not in constructor!
    if(!inCtor)
    {
        CEF_REQUIRE_IO_THREAD();
    }

    for(int i = 0; i < size; ++i)
    {
        PointerType handler(new HandlerType([this](const Id& id) { onRequestHandlerFinished(id); }));
        m_resourceHandlers.emplace(handler->getId(), handler);
        m_unusedHandlers.push_back(handler);
    }
}

//! Callback which is called when a usued request handler has finished.
template<typename HandlerType>
void ResourceHandlerPool<HandlerType>::onRequestHandlerFinished(const Id& id)
{
    CEF_REQUIRE_IO_THREAD();

    auto handlerIt = m_resourceHandlers.find(id);
    EXECGRAPH_ASSERT(handlerIt != m_resourceHandlers.end(), "Handler is not known!");

    if(handlerIt != m_resourceHandlers.end())
    {
        auto& handler = handlerIt->second;
        EXECGRAPH_ASSERT(std::find_if(m_unusedHandlers.begin(),
                                      m_unusedHandlers.end(),
                                      [&id](auto& h) { return h->getId() == id; }) == m_unusedHandlers.end(),
                         "Request handler: " << id.getFullName() << "is in the unused set!");

        EXECGRAPH_ASSERT(!handler->isUsed(), "Handler should now be unused!");

        // Put handler in the unused set.
        m_unusedHandlers.push_back(handler);
    }
}

//! Checkout an unused request handler.
//! @param nullptr if not succesful.
template<typename HandlerType>
typename ResourceHandlerPool<HandlerType>::PointerType
ResourceHandlerPool<HandlerType>::checkoutUnusuedHandler()
{
    CEF_REQUIRE_IO_THREAD();

    if(!m_unusedHandlers.size())
    {
        // To little request handlers available, allocate some more.
        batchAllocateHandlers();
    }

    EXECGRAPH_ASSERT(m_unusedHandlers.size(), "There should be now some unused handlers.")
    if(m_unusedHandlers.size())
    {
        // Checkout the first and set it to used!
        auto handler = m_unusedHandlers.front();
        m_unusedHandlers.pop_front();
        handler->setUsed(true);
        return handler;
    }
    return nullptr;
}

#endif