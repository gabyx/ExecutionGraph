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

#ifndef executionGraphGUI_backend_requestHandlers_GraphManipulationRequestHandler_hpp
#define executionGraphGUI_backend_requestHandlers_GraphManipulationRequestHandler_hpp

#include "executionGraphGUI/backend/BackendRequestHandler.hpp"
#include "executionGraphGUI/common/FunctionMap.hpp"

class ExecutionGraphBackend;

/* ---------------------------------------------------------------------------------------*/
/*!
    Request handler for manipulation operations on graphs in the backend.

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
    using IdNamed  = BackendRequestHandler::IdNamed;
    using Function = std::function<void(GraphManipulationRequestHandler&,
                                        const Request& request,
                                        ResponsePromise& response)>;

public:
    GraphManipulationRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                                    const IdNamed& id = IdNamed("GraphManipulationRequestHandler"));

    void handleRequest(const Request& request, ResponsePromise& response) override;
    const std::unordered_set<std::string>& getRequestTypes() const override;

private:
    void handleAddNode(const Request& request,
                       ResponsePromise& response);
    void handleRemoveNode(const Request& request,
                          ResponsePromise& response);

private:
    static FunctionMap<Function> initFunctionMap();
    static const FunctionMap<Function> m_functionMap;

    std::shared_ptr<ExecutionGraphBackend> m_backend;
};

#endif
