//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 15 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#include "backend/DummyBackendMsgHandler.hpp"
#include <executionGraph/common/Log.hpp>
#include <iostream>

bool DummyBackendMsgHandler::OnQuery(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     int64 query_id,
                                     const CefString& request,
                                     bool persistent,
                                     CefRefPtr<CefMessageRouterBrowserSide::Callback> callback)
{
    static_assert(sizeof(std::remove_pointer_t<decltype(request.c_str())>) == 2 * sizeof(unsigned char), "wups");
    auto* p = reinterpret_cast<const unsigned char*>(request.c_str());
    EXECGRAPH_LOGMSG(TRACE, "Received data: (binary): [");
    for(int i = 0; i < request.length() * 2; ++i)  // *2 because char16_t
    {
        EXECGRAPH_LOGMSG(TRACE, static_cast<unsigned int>(p[i]) << " , ");
    }
    EXECGRAPH_LOGMSG(TRACE, "]" << std::endl);

    std::string res = request.ToString();
    auto result     = "received: " + request.ToString();
    callback->Success(result);
    return true;
}
