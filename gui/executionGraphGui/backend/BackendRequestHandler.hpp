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

#ifndef executionGraphGui_backend_BackendRequestHandler_hpp
#define executionGraphGui_backend_BackendRequestHandler_hpp

#include <executionGraph/common/IObjectID.hpp>
#include <memory>
#include <rttr/type>
#include "backend/Backend.hpp"
#include "cefapp/Request.hpp"
#include "cefapp/Response.hpp"

class BackendRequestHandler : public executionGraph::IObjectID
{
    RTTR_ENABLE()
    EXECGRAPH_OBJECT_ID_DECLARATION

public:
    BackendRequestHandler(const Id& id)
        : m_id(id) {}

    BackendRequestHandler(const BackendRequestHandler&) = delete;
    BackendRequestHandler& operator=(const BackendRequestHandler&) = delete;

    virtual bool handleRequest(const Request& m_request, ResponsePromise& m_response) = 0;
};

#endif