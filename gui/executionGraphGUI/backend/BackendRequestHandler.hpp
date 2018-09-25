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

#ifndef executionGraphGUI_backend_BackendRequestHandler_hpp
#define executionGraphGUI_backend_BackendRequestHandler_hpp

#include <memory>
#include <unordered_set>
#include <rttr/type>
#include <executionGraph/common/IObjectID.hpp>
#include "executionGraphGUI/backend/Backend.hpp"
#include "executionGraphGUI/common/Assert.hpp"
#include "executionGraphGUI/common/Request.hpp"
#include "executionGraphGUI/common/Response.hpp"

class BackendRequestHandler : public executionGraph::IObjectID
{
    RTTR_ENABLE()
    EXECGRAPH_NAMED_OBJECT_ID_DECLARATION

public:
    BackendRequestHandler(const IdNamed& id)
        : m_id(id) {}

    BackendRequestHandler(const BackendRequestHandler&) = delete;
    BackendRequestHandler& operator=(const BackendRequestHandler&) = delete;

    //! Return all request types which this handler will handle.
    virtual const std::unordered_set<std::string>& getRequestTypes() const = 0;

    //! Handle the `request` by resolving the response promise `response`.
    virtual void handleRequest(const Request& request, ResponsePromise& response) = 0;
};

#endif