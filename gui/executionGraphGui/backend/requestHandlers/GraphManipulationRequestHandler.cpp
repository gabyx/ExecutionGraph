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

#include "executionGraphGui/backend/requestHandlers/GraphManipulationRequestHandler.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraphGui/backend/ExecutionGraphBackend.hpp"
#include "executionGraphGui/backend/ExecutionGraphBackendDefs.hpp"
#include "executionGraphGui/backend/requestHandlers/RequestHandlerCommon.hpp"
#include "executionGraphGui/common/AllocatorProxyFlatBuffer.hpp"
#include "executionGraphGui/common/Loggers.hpp"
#include "executionGraphGui/common/RequestError.hpp"
#include "executionGraphGui/messages/schemas/cpp/GraphManipulationMessages_generated.h"

namespace fl = flatbuffers;
namespace s  = executionGraphGui::serialization;

//! Init the function mapping.
FunctionMap<GraphManipulationRequestHandler::Function> GraphManipulationRequestHandler::initFunctionMap()
{
    using Entry = typename FunctionMap<Function>::Entry;

    auto r = {Entry(targetBase / "graph/addNode", Function(&GraphManipulationRequestHandler::handleAddNode)),
              Entry(targetBase / "graph/removeNode", Function(&GraphManipulationRequestHandler::handleRemoveNode)),
              Entry(targetBase / "graph/addConnection", Function(&GraphManipulationRequestHandler::handleAddConnection)),
              Entry(targetBase / "graph/removeConnection", Function(&GraphManipulationRequestHandler::handleRemoveConnection))};
    return {r};
}

//! Static handler map: request to handler function mapping.
const FunctionMap<GraphManipulationRequestHandler::Function> GraphManipulationRequestHandler::m_functionMap = GraphManipulationRequestHandler::initFunctionMap();

//! Konstructor.
GraphManipulationRequestHandler::GraphManipulationRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                                                                 const IdNamed& id)
    : BackendRequestHandler(id), m_backend(backend)
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
    auto it = m_functionMap.m_map.find(request.getTarget().string());
    if(it != m_functionMap.m_map.end())
    {
        it->second(*this, request, response);
    }
}

//! Handle the operation of adding a node.
void GraphManipulationRequestHandler::handleAddNode(const Request& request,
                                                    ResponsePromise& response)
{
    // Request validation
    auto& payload = request.getPayload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt,
                                      "Request data is null!");

    auto nodeReq = getRootOfPayloadAndVerify<s::AddNodeRequest>(*payload);

    Id graphID{nodeReq->graphId()->str()};

    // Callback to create the response
    auto responseCreator = [&response](auto& graph, auto& node) {
        using Allocator = ResponsePromise::Allocator;
        AllocatorProxyFlatBuffer<Allocator> allocator(response.getAllocator());
        flatbuffers::FlatBufferBuilder builder(512, &allocator);

        using GraphType      = typename std::remove_cv_t<std::remove_reference_t<decltype(graph)>>;
        using Config         = typename GraphType::Config;
        using NodeSerializer = typename ExecutionGraphBackendDefs<Config>::NodeSerializer;

        // Serialize the response
        NodeSerializer serializer;
        auto nodeOffset = serializer.write(builder, node, false, true);

        s::AddNodeResponseBuilder addResponse(builder);
        addResponse.add_node(nodeOffset);
        auto resOff = addResponse.Finish();
        builder.Finish(resOff);

        // Set the response.
        response.setReady(ResponsePromise::Payload{releaseIntoBinaryBuffer(std::move(allocator),
                                                                           builder),
                                                   "application/octet-stream"});
    };

    // Execute the request
    m_backend->addNode(graphID,
                       nodeReq->node()->type()->str(),
                       nodeReq->node()->name()->str(),
                       responseCreator);
}

//! Handle the operation of removing a node.
void GraphManipulationRequestHandler::handleRemoveNode(const Request& request,
                                                       ResponsePromise& response)
{
    // Request validation
    auto& payload = request.getPayload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt,
                                      "Request data is null!");

    auto nodeReq = getRootOfPayloadAndVerify<s::RemoveNodeRequest>(*payload);

    Id graphID{nodeReq->graphId()->str()};

    // Execute the request
    m_backend->removeNode(graphID,
                          nodeReq->nodeId());

    // Set the response.
    response.setReady();
}

//! Handle the operation of adding a connection.
void GraphManipulationRequestHandler::handleAddConnection(const Request& request,
                                                          ResponsePromise& response)
{
    // Request validation
    auto& payload = request.getPayload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt,
                                      "Request data is null!");

    auto connectionReq = getRootOfPayloadAndVerify<s::AddConnectionRequest>(*payload);

    Id graphID{connectionReq->graphId()->str()};

    auto responseCreator = [](auto& graph, auto&& cycles) {

    };

    // Execute the request
    auto socketLink = connectionReq->socketLink();
    m_backend->addConnection(graphID,
                             socketLink->outNodeId(),
                             socketLink->outSocketIdx(),
                             socketLink->inNodeId(),
                             socketLink->inSocketIdx(),
                             socketLink->isWriteLink(),
                             connectionReq->checkForCycles(),
                             responseCreator);

    // Set the response.
    response.setReady();
}

//! Handle the operation of removing a connection.
void GraphManipulationRequestHandler::handleRemoveConnection(const Request& request,
                                                             ResponsePromise& response)
{
    // Request validation
    auto& payload = request.getPayload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt,
                                      "Request data is null!");

    auto connectionReq = getRootOfPayloadAndVerify<s::RemoveConnectionRequest>(*payload);

    Id graphID{connectionReq->graphId()->str()};

    // Execute the request
    auto socketLink = connectionReq->socketLink();
    m_backend->removeConnection(graphID,
                                socketLink->outNodeId(),
                                socketLink->outSocketIdx(),
                                socketLink->inNodeId(),
                                socketLink->inSocketIdx(),
                                socketLink->isWriteLink());

    // Set the response.
    response.setReady();
}
