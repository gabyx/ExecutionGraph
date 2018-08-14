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
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraphGUI/backend/ExecutionGraphBackend.hpp"
#include "executionGraphGUI/backend/ExecutionGraphBackendDefs.hpp"
#include "executionGraphGUI/backend/requestHandlers/RequestHandlerCommon.hpp"
#include "executionGraphGUI/common/AllocatorProxyFlatBuffer.hpp"
#include "executionGraphGUI/common/Loggers.hpp"
#include "executionGraphGUI/common/RequestError.hpp"
#include "executionGraphGUI/messages/schemas/cpp/GraphManipulationMessages_generated.h"

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
    auto it = m_functionMap.m_map.find(request.getURL().string());
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
    auto* payload = request.getPayload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == nullptr,
                                      "Request data is null!");

    auto nodeReq = getRootOfPayloadAndVerify<s::AddNodeRequest>(*payload);

    Id graphID{nodeReq->graphId()->str()};

    // Callback to create the response
    auto responseCreator = [&response, graphID](auto& graph, auto& node) {
        using Allocator = ResponsePromise::Allocator;
        AllocatorProxyFlatBuffer<Allocator> allocator(response.getAllocator());
        flatbuffers::FlatBufferBuilder builder(512, &allocator);

        using GraphType      = std::remove_cv_t<std::remove_reference_t<decltype(graph)>>;
        using Config         = typename GraphType::Config;
        using NodeSerializer = typename ExecutionGraphBackendDefs<Config>::NodeSerializer;

        // Serialize the response
        NodeSerializer serializer;
        auto nodeOffset = serializer.write(builder, node);

        s::AddNodeResponseBuilder addResponse(builder);
        addResponse.add_node(nodeOffset);
        auto resOff = addResponse.Finish();
        builder.Finish(resOff);

        // Set the response.
        auto detachedBuffer = builder.Release();
        response.setReady(ResponsePromise::Payload{makeBinaryBuffer(std::move(allocator),
                                                                    std::move(detachedBuffer)),
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
    auto* payload = request.getPayload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == nullptr,
                                      "Request data is null!");

    auto nodeReq = getRootOfPayloadAndVerify<s::RemoveNodeRequest>(*payload);

    Id graphID{nodeReq->graphId()->str()};

    // Execute the request
    m_backend->removeNode(graphID,
                          nodeReq->nodeId());
}