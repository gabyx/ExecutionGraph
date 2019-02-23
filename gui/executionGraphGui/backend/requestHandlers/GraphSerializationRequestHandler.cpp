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

#include "executionGraphGui/backend/requestHandlers/GraphSerializationRequestHandler.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraphGui/backend/ExecutionGraphBackend.hpp"
#include "executionGraphGui/backend/ExecutionGraphBackendDefs.hpp"
#include "executionGraphGui/backend/requestHandlers/RequestHandlerCommon.hpp"
#include "executionGraphGui/common/AllocatorProxyFlatBuffer.hpp"
#include "executionGraphGui/common/Loggers.hpp"
#include "executionGraphGui/common/RequestError.hpp"
#include "executionGraphGui/messages/schemas/cpp/GraphSerializationMessages_generated.h"

namespace fl = flatbuffers;
namespace s  = executionGraphGui::serialization;

//! Init the function mapping.
GraphSerializationRequestHandler::FuncMap GraphSerializationRequestHandler::initFunctionMap()
{
    using Entry = typename FuncMap::Entry;

    auto r = {Entry(targetBase / "general/saveGraph", Function(&GraphSerializationRequestHandler::handleSaveGraph)),
              Entry(targetBase / "general/loadGraph", Function(&GraphSerializationRequestHandler::handleLoadGraph))};
    return {r};
}

//! Static handler map: request to handler function mapping.
const GraphSerializationRequestHandler::FuncMap GraphSerializationRequestHandler::m_functionMap =
    GraphSerializationRequestHandler::initFunctionMap();

//! Konstructor.
GraphSerializationRequestHandler::GraphSerializationRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                                                                   const IdNamed& id)
    : BackendRequestHandler(id), m_backend(backend)
{}

//! Get the request types for which this handler is registered.
const std::unordered_set<GraphSerializationRequestHandler::HandlerKey>& GraphSerializationRequestHandler::requestTargets()
    const
{
    return m_functionMap.keys();
}

//! Handle the operation of adding a graph.
void GraphSerializationRequestHandler::handleRequest(const Request& request,
                                                     ResponsePromise& response)
{
    EXECGRAPHGUI_BACKENDLOG_INFO("GraphSerializationRequestHandler::handleRequest");
    m_functionMap.dispatch(request.target().native(), *this, request, response);
}

//! Handle the operation of saving a graph to a file.
void GraphSerializationRequestHandler::handleSaveGraph(const Request& request,
                                                       ResponsePromise& response)
{
    // Request validation
    auto& payload = request.payload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt, "Request data is null!");

    auto saveReq = getRootOfPayloadAndVerify<s::SaveGraphRequest>(*payload);

    auto vis     = saveReq->visualization();
    auto visView = vis ? BinaryBufferView(vis->data(), vis->size())
                       : BinaryBufferView{};

    m_backend->saveGraph(Id{saveReq->graphId()->c_str()},
                         saveReq->filePath()->c_str(),
                         saveReq->overwrite(),
                         visView);

    response.setReady();
}

//! Handle the operation of loading a graph from a file.
void GraphSerializationRequestHandler::handleLoadGraph(const Request& request,
                                                       ResponsePromise& response)
{
    // Request validation
    auto& payload = request.payload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt,
                                      "Request data is null!");

    auto loadReq = getRootOfPayloadAndVerify<s::LoadGraphRequest>(*payload);

    // Callback to create the response
    auto responseCreator = [&response](const auto& graphId,
                                       const auto& graph,
                                       const auto& graphDescription,
                                       BinaryBufferView visualization) {
        using Allocator = ResponsePromise::Allocator;
        AllocatorProxyFlatBuffer<Allocator> allocator(response.getAllocator());
        flatbuffers::FlatBufferBuilder builder(10 * (1 << 20), &allocator);

        using GraphType       = std::decay_t<decltype(graph)>;
        using Config          = typename GraphType::Config;
        using NodeSerializer  = typename ExecutionGraphBackendDefs<Config>::NodeSerializer;
        using GraphSerializer = typename ExecutionGraphBackendDefs<Config>::GraphSerializer;

        // Serialize the graph.
        NodeSerializer nodeSerializer;
        GraphSerializer graphSerializer(nodeSerializer);
        auto graphOffset = graphSerializer.writeGraph(builder,
                                                      graph,
                                                      graphDescription,
                                                      visualization);

        auto graphIdOff = builder.CreateString(graphId.toString());

        s::LoadGraphResponseBuilder loadResponse(builder);
        loadResponse.add_graphId(graphIdOff);
        loadResponse.add_graph(graphOffset);
        auto resOff = loadResponse.Finish();
        builder.Finish(resOff);

        // Set the response.
        response.setReady(ResponsePromise::Payload{releaseIntoBinaryBuffer(std::move(allocator),
                                                                           builder),
                                                   "application/octet-stream"});
    };

    m_backend->loadGraph(loadReq->filePath()->c_str(),
                         responseCreator);
}