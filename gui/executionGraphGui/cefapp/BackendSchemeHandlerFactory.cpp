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

#include "BackendSchemeHandlerFactory.hpp"
#include <executionGraph/common/Assert.hpp>
#include <executionGraph/common/Exception.hpp>
#include "cefapp/Loggers.hpp"
#include "cefapp/SchemeHandlerHelper.hpp"

CefRefPtr<CefResourceHandler> BackendSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                                                  CefRefPtr<CefFrame> frame,
                                                                  const CefString& scheme_name,
                                                                  CefRefPtr<CefRequest> request)
{
    CEF_REQUIRE_IO_THREAD();
    auto handler = m_pool.checkoutUnusuedHandler();
    if(!handler)
    {
        EXECGRAPHGUI_APPLOG_ERROR("BackendSchemeHandlerFactory: No unused handler could be found.");
    }
    return handler;
}
