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

#ifndef executionGraphGui_backend_DummyBackendMsgHandler_hpp
#define executionGraphGui_backend_DummyBackendMsgHandler_hpp

#include "backend/BackendMessageHandler.hpp"

class DummyBackendMsgHandler final : public BackendMessageHandler
{
    RTTR_ENABLE(BackendMessageHandler);

public:
    using Id = BackendMessageHandler::Id;

public:
    template<typename... Args>
    DummyBackendMsgHandler(const Id& id = "DummyBackendMsgHandler", Args&&... args)
        : BackendMessageHandler(std::forward<Args>(args)...)
    {
    }

    bool OnQuery(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int64 query_id,
                 const CefString& request,
                 bool persistent,
                 CefRefPtr<Callback> callback) override;
};

#endif
