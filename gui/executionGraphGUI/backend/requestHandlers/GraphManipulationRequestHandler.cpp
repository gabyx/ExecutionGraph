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

#include "executionGraphGUI/backend/requestHandlers/GraphManipulationRequestHandler.hpp"
#include <chrono>
#include <vector>
#include "executionGraphGUI/backend/ExecutionGraphBackend.hpp"
#include "executionGraphGUI/common/AllocatorProxyFlatBuffer.hpp"
#include "executionGraphGUI/common/Loggers.hpp"
#include "executionGraphGUI/common/RequestError.hpp"
#include "executionGraphGUI/messages/schemas/GraphManipulationMessages_generated.h"

namespace fl = flatbuffers;
namespace s  = executionGraphGUI::serialization;

//! Init the function mapping.
FunctionMap<GraphManipulationRequestHandler::Function> GraphManipulationRequestHandler::initFunctionMap()
{
    using Entry = typename FunctionMap<Function>::Entry;

    auto r = {Entry("graph/addNode", Function(&GraphManipulationRequestHandler::handleAddNode)),
              Entry("graph/removeNode", Function(&GraphManipulationRequestHandler::handleRemoveNode))};
    return {r};
}

//! Static handler map: request to handler function mapping.
const FunctionMap<gandler::Function> GraphManipulationRequestHandler::m_functionMap = GraphManipulationRequestHandler::initFunctionMap();

//! Konstructor.
GraphManipulationRequestHandler::GraphManipulationRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                                                                 const IdNamed& id)
    : BackendRequestHandler(id)
{
}

//! Get the request types for which this handler is registered.
const std::unordered_set<std::string>& GraphManipulationRequestHandler::getRequestTypes() const
{
    return m_functionMap.m_keys;
}

//! Handle the request.
void GraphManipulationRequestHandler::handleRequest(const Request& request,
                                                    ResponsePromise& response)
{
    EXECGRAPHGUI_BACKENDLOG_INFO("GraphManipulationRequestHandler::handleRequest");

    // Dispatch to the correct function
    auto it = m_functionMap.m_map.find(request.getURL().string());
    if(it != m_functionMap.m_map.end())
    {
        it->second(*this, request, response);
    }
}

//! Handle the "graph/addNode"
void GraphManipulationRequestHandler::handleAddNode(const Request& request,
                                                    ResponsePromise& response)
{
    // Request validation
    EXECGRAPHGUI_THROW_EXCEPTION_TYPE_IF(request->getPayload() == nullptr,
                                         "Request data is null!",
                                         BadRequestError);
}

//! Handle the "graph/removeNode"
void GraphManipulationRequestHandler::handleRemoveNode(const Request& request,
                                                       ResponsePromise& response)
{
    // Request validation
    EXECGRAPHGUI_THROW_EXCEPTION_TYPE_IF(request->getPayload() == nullptr,
                                         "Request data is null!",
                                         BadRequestError);
}