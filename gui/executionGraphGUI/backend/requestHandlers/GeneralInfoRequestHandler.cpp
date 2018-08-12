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

#include "executionGraphGUI/backend/requestHandlers/GeneralInfoRequestHandler.hpp"
#include <chrono>
#include <vector>
#include <executionGraph/serialization/GraphTypeDescriptionSerializer.hpp>
#include "executionGraphGUI/backend/ExecutionGraphBackend.hpp"
#include "executionGraphGUI/common/AllocatorProxyFlatBuffer.hpp"
#include "executionGraphGUI/common/Exception.hpp"
#include "executionGraphGUI/common/Loggers.hpp"
#include "executionGraphGUI/messages/schemas/cpp/GeneralInfoMessages_generated.h"

using namespace executionGraph;
namespace fl = flatbuffers;
namespace s  = executionGraph::serialization;
namespace sG = executionGraphGUI::serialization;

//! Init the function mapping.
FunctionMap<GeneralInfoRequestHandler::Function> GeneralInfoRequestHandler::initFunctionMap()
{
    using Entry = typename FunctionMap<Function>::Entry;
    auto r      = {Entry{"general/getAllGraphTypeDescriptions",
                    Function{&GeneralInfoRequestHandler::handleGetAllGraphTypeDescriptions}}};
    return {r};
}

//! Static handler map: request to handler function mapping.
const FunctionMap<GeneralInfoRequestHandler::Function> GeneralInfoRequestHandler::m_functionMap = GeneralInfoRequestHandler::initFunctionMap();

//! Konstructor.
GeneralInfoRequestHandler::GeneralInfoRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                                                     const IdNamed& id)
    : BackendRequestHandler(id)
{
}

//! Get the request types for which this handler is registered.
const std::unordered_set<std::string>& GeneralInfoRequestHandler::getRequestTypes() const
{
    return m_functionMap.m_keys;
}

//! Handle the request.
void GeneralInfoRequestHandler::handleRequest(const Request& request,
                                              ResponsePromise& response)
{
    EXECGRAPHGUI_BACKENDLOG_INFO("GeneralInfoRequestHandler::handleRequest");

    // Dispatch to the correct function
    auto it = m_functionMap.m_map.find(request.getURL().string());
    if(it != m_functionMap.m_map.end())
    {
        it->second(*this, request, response);
    }
}

//! Handle the "GetAllGraphTypeDescriptions"
void GeneralInfoRequestHandler::handleGetAllGraphTypeDescriptions(const Request& request,
                                                                  ResponsePromise& response)
{
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(request.getPayload() != nullptr,
                                      "There should not be any request payload for this request");
    using Allocator = ResponsePromise::Allocator;

    AllocatorProxyFlatBuffer<Allocator> allocator(response.getAllocator());
    flatbuffers::FlatBufferBuilder builder(1024, &allocator);

    // Serialize the response
    std::vector<flatbuffers::Offset<s::GraphTypeDescription>> graphs;

    for(auto& kV : m_backend->getGraphTypeDescriptions())
    {
        auto& graphDesc = kV.second;
        graphs.emplace_back(GraphTypeDescriptionSerializer::write(builder, graphDesc));
    }

    auto offset = sG::CreateGetAllGraphTypeDescriptionsResponseDirect(builder, &graphs);
    builder.Finish(offset);

    // Set the response.
    auto detachedBuffer = builder.Release();
    response.setReady(ResponsePromise::Payload{makeBinaryBuffer(std::move(allocator),
                                                                std::move(detachedBuffer)),
                                               "application/octet-stream"});
}