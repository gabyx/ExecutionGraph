//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 08 2018
//!  @author Gabriel Nützi, <gnuetzi (at) gmail (døt) com>
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef cefapp_BackendSchemeHandlerFactory_hpp
#define cefapp_BackendSchemeHandlerFactory_hpp

#include <cef_scheme.h>
#include <executionGraph/common/FileSystem.hpp>
#include <foonathan/memory/heap_allocator.hpp>
#include <foonathan/memory/memory_pool.hpp>
#include <queue>
#include <vector>
#include "cefapp/BackendResourceHandler.hpp"
#include "cefapp/BufferPool.hpp"
#include "cefapp/ResourceHandlerPool.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    Factory for creating client request handlers.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class BackendSchemeHandlerFactory final : public CefSchemeHandlerFactory
{
    IMPLEMENT_REFCOUNTING(BackendSchemeHandlerFactory)

public:
    BackendSchemeHandlerFactory(const std::path& pathPrefix = "")
        : m_pathPrefix(pathPrefix)
        , m_bufferPool(std::make_shared<BufferPool>())
        , m_handlerPool(sizeof(BackendResourceHandler), sizeof(BackendResourceHandler) * 30)
    {
    }
    virtual ~BackendSchemeHandlerFactory() = default;

    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                                 CefRefPtr<CefFrame> frame,
                                                 const CefString& scheme_name,
                                                 CefRefPtr<CefRequest> request) override;

private:
    const std::path m_pathPrefix;

private:
    std::shared_ptr<BufferPool> m_bufferPool;        //! Binary buffers for the messages.
    foonathan::memory::memory_pool<> m_handlerPool;  //! Simple request handler pool.
};

#endif
