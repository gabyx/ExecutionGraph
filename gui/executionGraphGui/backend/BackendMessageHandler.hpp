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

#ifndef executionGraphGui_backend_BackendMessageHandler_hpp
#define executionGraphGui_backend_BackendMessageHandler_hpp

#include <wrapper/cef_message_router.h>
#include <executionGraph/common/ObjectID.hpp>

class BackendMessageHandler : public CefMessageRouterBrowserSide::Handler,
                              public executionGraph::ObjectID
{
public:
    using Id = executionGraph::ObjectID::Id;
public:
    BackendMessageHandler(const Id& id) : executionGraph::ObjectID(id) {}

    // Called due to cefQuery execution in message_router.html.
    virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int64 query_id,
                         const CefString& request,
                         bool persistent,
                         CefRefPtr<Callback> callback) override;

private:
    DISALLOW_COPY_AND_ASSIGN(BackendMessageHandler);
};

#endif