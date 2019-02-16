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

#pragma once

#include "executionGraph/common/FileSystem.hpp"
#include "executionGraphGui/backend/BackendRequestHandler.hpp"
#include "executionGraphGui/common/FunctionMap.hpp"

class ExecutionGraphBackend;

/* ---------------------------------------------------------------------------------------*/
/*!
    Request handler for manipulation operations on graphs in the backend.

    Handles the request URLs: 
        - "/eg-backend/graph/addNodes"
        - "/eg-backend/graph/removeNodes"

        - "/eg-backend/graph/addConnection"
        - "/eg-backend/graph/removeConnection"

    @date Sat Jul 07 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/

class GraphManipulationRequestHandler final : public BackendRequestHandler
{
    RTTR_ENABLE(BackendRequestHandler)

public:
    using IdNamed  = BackendRequestHandler::IdNamed;
    using Function = std::function<void(GraphManipulationRequestHandler&,
                                        const Request& request,
                                        ResponsePromise& response)>;
    using FuncMap  = FunctionMap<Function, HandlerKey>;

public:
    GraphManipulationRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                                    const IdNamed& id = IdNamed("GraphManipulationRequestHandler"));

    void handleRequest(const Request& request, ResponsePromise& response) override;
    const std::unordered_set<HandlerKey>& requestTargets() const override;

private:
    void handleAddNode(const Request& request,
                       ResponsePromise& response);
    void handleRemoveNode(const Request& request,
                          ResponsePromise& response);

    void handleAddConnection(const Request& request,
                             ResponsePromise& response);
    void handleRemoveConnection(const Request& request,
                                ResponsePromise& response);

private:
    static FuncMap initFunctionMap();
    static const FuncMap m_functionMap;

    std::shared_ptr<ExecutionGraphBackend> m_backend;
};
