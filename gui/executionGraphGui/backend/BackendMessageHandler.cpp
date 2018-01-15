//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//! 
//!  @date Sun Jan 14 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//! 
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#include "backend/BackendMessageHandler.hpp"

bool BackendMessageHandler::OnQuery(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             int64 query_id,
                             const CefString& request,
                             bool persistent,
                             CefRefPtr<Callback> callback)
{
    std::string result = request;
    result             = "received: " + result;
    callback->Success(result);
    return true;
}
