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
#include <queue>
#include <vector>
#include "cefapp/BackendResourceHandler.hpp"
#include "cefapp/ResourceHandlerPool.hpp"

//! Factory for creating client request handlers.
class BackendSchemeHandlerFactory final : public CefSchemeHandlerFactory
{
    IMPLEMENT_REFCOUNTING(BackendSchemeHandlerFactory);

public:
    BackendSchemeHandlerFactory(const std::path& pathPrefix = "")
        : m_pathPrefix(pathPrefix)
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
    ResourceHandlerPool<BackendResourceHandler> m_pool;  //! Simple request handler pool.
};

#endif
