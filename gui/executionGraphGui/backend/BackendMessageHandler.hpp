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

#include <executionGraph/common/IObjectID.hpp>
#include <memory>
#include <rttr/type.h>
#include <wrapper/cef_message_router.h>
#include "backend/Backend.hpp"

class BackendMessageHandler : public CefMessageRouterBrowserSide::Handler,
                              public executionGraph::IObjectID
{
    RTTR_ENABLE()
    EXECGRAPH_OBJECT_ID_DECLARATION

public:
    BackendMessageHandler(const Id& id,
                          const std::shared_ptr<Backend>& backend)
        : m_id(id), m_backend(backend) {}

    // Called due to cefQuery execution in message_router.html.
    virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int64 query_id,
                         const CefString& request,
                         bool persistent,
                         CefRefPtr<Callback> callback) override = 0;

private:
    std::shared_ptr<Backend> m_backend;  //! The backend which is called from this message handler.

private:
    DISALLOW_COPY_AND_ASSIGN(BackendMessageHandler);
};

#endif