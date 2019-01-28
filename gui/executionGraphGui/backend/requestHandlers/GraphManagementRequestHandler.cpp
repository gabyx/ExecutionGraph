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
GraphManagementRequestHandler::FuncMap GraphManagementRequestHandler::initFunctionMap()
{
    using Entry = typename FuncMap::Entry;

    auto r = {Entry(targetBase / "general/addGraph",
                    Function(&GraphManagementRequestHandler::handleAddGraph)),
              Entry(targetBase / "general/removeGraph",
                    Function(&GraphManagementRequestHandler::handleRemoveGraph)),
              Entry(targetBase / "general/saveGraph",
                    Function(&GraphManagementRequestHandler::handleSaveGraph)),
              Entry(targetBase / "general/loadGraph",
                    Function(&GraphManagementRequestHandler::handleLoadGraph))};
    return {r};
}

//! Static handler map: request to handler function mapping.
const GraphManagementRequestHandler::FuncMap GraphManagementRequestHandler::m_functionMap =
    GraphManagementRequestHandler::initFunctionMap();

//! Konstructor.
GraphManagementRequestHandler::GraphManagementRequestHandler(
    std::shared_ptr<ExecutionGraphBackend> backend, const IdNamed& id)
    : BackendRequestHandler(id), m_backend(backend)
{}

//! Get the request types for which this handler is registered.
const std::unordered_set<GraphManagementRequestHandler::HandlerKey>&
GraphManagementRequestHandler::requestTargets() const
{
    return m_functionMap.keys();
}

//! Handle the operation of adding a graph.
void GraphManagementRequestHandler::handleRequest(const Request& request, ResponsePromise& response)
{
    EXECGRAPHGUI_BACKENDLOG_INFO("GraphManagementRequestHandler::handleRequest");
    m_functionMap.dispatch(request.target().native(), *this, request, response);
}

//! Handle the operation of adding a graph.
void GraphManagementRequestHandler::handleAddGraph(const Request& request,
                                                   ResponsePromise& response)
{
    // Request validation
    auto& payload = request.payload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt, "Request data is null!");

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
    response.setReady(ResponsePromise::Payload{
        releaseIntoBinaryBuffer(std::move(allocator), builder), "application/octet-stream"});
}

//! Handle the operation of removing a graph.
void GraphManagementRequestHandler::handleRemoveGraph(const Request& request,
                                                      ResponsePromise& response)
{
    // Request validation
    auto& payload = request.payload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt, "Request data is null!");

    auto nodeReq = getRootOfPayloadAndVerify<s::RemoveGraphRequest>(*payload);

    Id graphID{nodeReq->graphId()->str()};

    // Execute the request
    m_backend->removeGraph(graphID);

    // Set the response ready
    response.setReady();
}

//! Handle the operation of saving a graph to a file.
void GraphManagementRequestHandler::handleSaveGraph(const Request& request,
                                                    ResponsePromise& response)
{
    // Request validation
    auto& payload = request.payload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt, "Request data is null!");

    auto saveReq = getRootOfPayloadAndVerify<s::SaveGraphRequest>(*payload);

    m_backend->saveGraph(Id{saveReq->graphId()->c_str()},
                         saveReq->filePath()->c_str(),
                         saveReq->overwrite());

    EXECGRAPHGUI_THROW_BAD_REQUEST("Not Implemented!");
}

//! Handle the operation of loading a graph from a file.
void GraphManagementRequestHandler::handleLoadGraph(const Request& request,
                                                    ResponsePromise& response)
{
    // Request validation
    auto& payload = request.payload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt, "Request data is null!");

    EXECGRAPHGUI_THROW_BAD_REQUEST("Not Implemented!");
}