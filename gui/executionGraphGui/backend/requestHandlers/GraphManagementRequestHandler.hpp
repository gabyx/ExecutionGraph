// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Tue Aug 14 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGui_backend_requestHandlers_GraphManagementRequestHandler_hpp
#define executionGraphGui_backend_requestHandlers_GraphManagementRequestHandler_hpp

#include "executionGraphGui/backend/BackendRequestHandler.hpp"
#include "executionGraphGui/common/FunctionMap.hpp"

class ExecutionGraphBackend;

/* ---------------------------------------------------------------------------------------*/
/*!
    Request handler for graph management operations in the backend. 

    Handles the request URLs: 
        - "/eg-backend/general/addGraph"
        - "/eg-backend/general/removeGraph"

    @date Sat Jul 07 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/

class GraphManagementRequestHandler final : public BackendRequestHandler
{
    RTTR_ENABLE(BackendRequestHandler)

public:
    using IdNamed  = BackendRequestHandler::IdNamed;
    using Function = std::function<void(GraphManagementRequestHandler&,
                                        const Request& request,
                                        ResponsePromise& response)>;

public:
    GraphManagementRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                                  const IdNamed& id = IdNamed("GraphManagementRequestHandler"));

    void handleRequest(const Request& request, ResponsePromise& response) override;
    const std::unordered_set<std::string>& getRequestTypes() const override;

private:
    void handleAddGraph(const Request& request,
                        ResponsePromise& response);
    void handleRemoveGraph(const Request& request,
                           ResponsePromise& response);

private:
    static FunctionMap<Function> initFunctionMap();
    static const FunctionMap<Function> m_functionMap;

    std::shared_ptr<ExecutionGraphBackend> m_backend;
};

#endif
