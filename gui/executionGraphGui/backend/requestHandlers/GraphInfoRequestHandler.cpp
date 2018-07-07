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

#include "backend/requestHandlers/GraphInfoRequestHandler.hpp"
#include <backend/ExecutionGraphBackend.hpp>
#include <chrono>
#include <common/Loggers.hpp>
#include <messages/schemas/GraphInfoMessages_generated.h>

//! Init the function mapping.
FunctionMap<GraphInfoRequestHandler::Function> GraphInfoRequestHandler::initFunctionMap()
{
    using Entry = typename FunctionMap<Function>::Entry;
    auto r      = {Entry{"general/getAllGraphTypeDescriptions",
                    Function{&GraphInfoRequestHandler::handleGetAllGraphTypeDescriptions}}};
    return {r};
}

//! Static handler map: request to handler function mapping.
const FunctionMap<GraphInfoRequestHandler::Function> GraphInfoRequestHandler::m_functionMap = GraphInfoRequestHandler::initFunctionMap();

//! Konstructor.
GraphInfoRequestHandler::GraphInfoRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                                                 const Id& id)
    : BackendRequestHandler(id)
{
}

//! Get the request types for which this handler is registered.
const std::unordered_set<std::string>& GraphInfoRequestHandler::getRequestTypes() const
{
    return m_functionMap.m_keys;
}

//! Handle the request.
void GraphInfoRequestHandler::handleRequest(const Request& request,
                                            ResponsePromise& response)
{
    EXECGRAPHGUI_BACKENDLOG_INFO("GraphInfoRequestHandler::handleRequest");

    // Dispatch to the correct function
    auto it = m_functionMap.m_map.find(request.getURL().string());
    if(it != m_functionMap.m_map.end())
    {
        it->second(*this, request, response);
    }
}

void GraphInfoRequestHandler::handleGetAllGraphTypeDescriptions(const Request& request,
                                                                ResponsePromise& response)
{
    EXECGRAPH_THROW_EXCEPTION_IF(request.getPayload() == nullptr,
                                 "There should not be any request payload for this request")
}