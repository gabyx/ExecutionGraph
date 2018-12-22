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

#include "executionGraphGui/backend/requestHandlers/GraphManagementRequestHandler.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraphGui/backend/ExecutionGraphBackend.hpp"
#include "executionGraphGui/backend/ExecutionGraphBackendDefs.hpp"
#include "executionGraphGui/backend/requestHandlers/RequestHandlerCommon.hpp"
#include "executionGraphGui/common/AllocatorProxyFlatBuffer.hpp"
#include "executionGraphGui/common/Loggers.hpp"
#include "executionGraphGui/common/RequestError.hpp"
#include "executionGraphGui/messages/schemas/cpp/GraphManagementMessages_generated.h"

namespace fl = flatbuffers;
namespace s  = executionGraphGui::serialization;

//! Init the function mapping.
FunctionMap<GraphManagementRequestHandler::Function> GraphManagementRequestHandler::initFunctionMap()
{
    using Entry = typename FunctionMap<Function>::Entry;

    auto r = {Entry(targetBase / "general/addGraph", Function(&GraphManagementRequestHandler::handleAddGraph)),
              Entry(targetBase / "general/removeGraph", Function(&GraphManagementRequestHandler::handleRemoveGraph))};
    return {r};
}

//! Static handler map: request to handler function mapping.
const FunctionMap<GraphManagementRequestHandler::Function> GraphManagementRequestHandler::m_functionMap = GraphManagementRequestHandler::initFunctionMap();

//! Konstructor.
GraphManagementRequestHandler::GraphManagementRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                                                             const IdNamed& id)
    : BackendRequestHandler(id), m_backend(backend)
{
}

//! Get the request types for which this handler is registered.
const std::unordered_set<std::string>& GraphManagementRequestHandler::getRequestTypes() const
{
    return m_functionMap.m_keys;
}

//! Handle the operation of adding a graph.
void GraphManagementRequestHandler::handleRequest(const Request& request,
                                                  ResponsePromise& response)
{
    EXECGRAPHGUI_BACKENDLOG_INFO("GraphManagementRequestHandler::handleRequest");

    // Dispatch to the correct function
    auto it = m_functionMap.m_map.find(request.getTarget().string());
    if(it != m_functionMap.m_map.end())
    {
        it->second(*this, request, response);
    }
}

//! Handle the operation of adding a graph.
void GraphManagementRequestHandler::handleAddGraph(const Request& request,
                                                   ResponsePromise& response)
{
    // Request validation
    auto& payload = request.getPayload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt,
                                      "Request data is null!");

    auto graphReq = getRootOfPayloadAndVerify<s::AddGraphRequest>(*payload);

    Id graphTypeId{graphReq->graphTypeId()->str()};

    // Execute the request
    Id graphId = m_backend->addGraph(graphTypeId);

    // Create the response
    using Allocator = ResponsePromise::Allocator;
    AllocatorProxyFlatBuffer<Allocator> allocator(response.getAllocator());
    flatbuffers::FlatBufferBuilder builder(16, &allocator);

    auto responseOff = s::CreateAddGraphResponseDirect(builder, graphId.toString().c_str());
    builder.Finish(responseOff);

    // Set the response ready
    response.setReady(ResponsePromise::Payload{releaseIntoBinaryBuffer(std::move(allocator),
                                                                       builder),
                                               "application/octet-stream"});
}

//! Handle the operation of removing a graph.
void GraphManagementRequestHandler::handleRemoveGraph(const Request& request,
                                                      ResponsePromise& response)
{
    // Request validation
    auto& payload = request.getPayload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt,
                                      "Request data is null!");

    auto nodeReq = getRootOfPayloadAndVerify<s::RemoveGraphRequest>(*payload);

    Id graphID{nodeReq->graphId()->str()};

    // Execute the request
    m_backend->removeGraph(graphID);

    // Set the response ready
    response.setReady();
}