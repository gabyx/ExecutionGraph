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

/* ---------------------------------------------------------------------------------------*/
/*!
    A stupid pool for resource handlers.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
template<typename THandlerFactory>
class ResourceHandlerPool final
{
public:
    using HandlerFactory = THandlerFactory;
    //! The Handler is of shared pointer type e.g. `CefRefPtr<...>`.
    using Handler = typename HandlerFactory::Handler;
    using Id      = executionGraph::Id;

public:
    template<typename... Args>
    ResourceHandlerPool(Args&&... args)
        : m_factory(std::forward<Args>(args)...)
    {
        batchAllocateHandlers<true>();
    }

    ~ResourceHandlerPool() = default;

    Handler checkoutUnusuedHandler();

private:
    std::deque<Handler> m_unusedHandlers;                //!< A FIFO queue of unused request handlers.
    std::unordered_map<Id, Handler> m_resourceHandlers;  //!< A pool of request handlers.
    HandlerFactory m_factory;                            //!< The factory which creates the handlers.
private:
    void onRequestHandlerFinished(const Id& id);

private:
    static const unsigned int c_batchHandlerAllocSize = 5;
    template<bool inCtor = false>
    void batchAllocateHandlers(unsigned int size = c_batchHandlerAllocSize);
};

//! Allocate some handlers.
template<typename THandlerFactory>
template<bool inCtor>
void ResourceHandlerPool<THandlerFactory>::batchAllocateHandlers(unsigned int size)
{
    // Only check thread if not in constructor!
    if(!inCtor)
    {
        CEF_REQUIRE_IO_THREAD();
    }

    for(int i = 0; i < size; ++i)
    {
        Handler handler = m_factory.create([this](const Id& id) { onRequestHandlerFinished(id); });
        m_resourceHandlers.emplace(handler->getId(), handler);
        m_unusedHandlers.push_back(handler);
    }

    EXECGRAPHGUI_APPLOG_DEBUG("ResourceHandlerPool: increased handler pool to '{0}'", m_resourceHandlers.size());
}

//! Callback which is called when a usued request handler has finished.
template<typename THandlerFactory>
void ResourceHandlerPool<THandlerFactory>::onRequestHandlerFinished(const Id& id)
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
                         "Request handler: " << id.getUniqueName() << "is in the unused set!");

        EXECGRAPH_ASSERT(!handler->isUsed(), "Handler should now be unused!");

        // Put handler in the unused set.
        m_unusedHandlers.push_back(handler);
        EXECGRAPHGUI_APPLOG_DEBUG("ResourceHandlerPool: finished: current unused handlers: '{0}'", m_unusedHandlers.size());
    }
}

//! Checkout an unused request handler.
//! @param nullptr if not succesful.
template<typename THandlerFactory>
typename ResourceHandlerPool<THandlerFactory>::Handler
ResourceHandlerPool<THandlerFactory>::checkoutUnusuedHandler()
{
    CEF_REQUIRE_IO_THREAD();

    EXECGRAPHGUI_APPLOG_DEBUG("ResourceHandlerPool: current unused handlers: '{0}'", m_unusedHandlers.size());

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