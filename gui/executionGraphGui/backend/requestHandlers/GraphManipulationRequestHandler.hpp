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

#ifndef executionGraphGui_backend_requestHandlers_GeneralInfoRequestHandler_hpp
#define executionGraphGui_backend_requestHandlers_GeneralInfoRequestHandler_hpp

#include "backend/BackendRequestHandler.hpp"
#include "common/FunctionMap.hpp"

class ExecutionGraphBackend;

/* ---------------------------------------------------------------------------------------*/
/*!
    Request Handler for information on graphs in the backend.

    Handles the request URLs: 
        - "graph/addNodes"
        - "graph/removeNodes"

    @date Sat Jul 07 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/

class GraphManipulationRequestHandler final : public BackendRequestHandler
{
    RTTR_ENABLE(BackendRequestHandler)

public:
    using Id       = BackendRequestHandler::Id;
    using Function = std::function<void(GraphManipulationRequestHandler&,
                                        const Request& request,
                                        ResponsePromise& response)>;

public:
    GraphManipulationRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                                    const Id& id = "GraphManipulationRequestHandler");

    void handleRequest(const Request& request, ResponsePromise& response) override;
    const std::unordered_set<std::string>& getRequestTypes() const override;

private:
    void handleAddNodes(const Request& request,
                        ResponsePromise& response);
    void handleRemoveNodes(const Request& request,
                           ResponsePromise& response);

private:
    static FunctionMap<Function> initFunctionMap();
    static const FunctionMap<Function> m_functionMap;

    std::shared_ptr<ExecutionGraphBackend> m_backend;
};

#endif
