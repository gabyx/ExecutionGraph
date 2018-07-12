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
#include <vector>
#include "common/AllocatorProxyFlatBuffer.hpp"

namespace fl = flatbuffers;
namespace s  = executionGraphGUI::serialization;

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

//! Handle the "GetAllGraphTypeDescriptions"
void GraphInfoRequestHandler::handleGetAllGraphTypeDescriptions(const Request& request,
                                                                ResponsePromise& response)
{
    EXECGRAPH_THROW_EXCEPTION_IF(request.getPayload() == nullptr,
                                 "There should not be any request payload for this request");
    using Allocator = ResponsePromise::Allocator;

    AllocatorProxyFlatBuffer<Allocator> allocator(*response.getAllocator());
    flatbuffers::FlatBufferBuilder builder(1024, &allocator);

    // Serialize the response
    std::vector<flatbuffers::Offset<s::GraphTypeDescription>> graphs;

    for(auto& kV : m_backend->getGraphTypeDescriptions())
    {
        auto& id                                          = kV.first;
        ExecutionGraphBackend::GraphTypeDescription& desc = kV.second;

        // Node descriptions
        std::vector<flatbuffers::Offset<s::NodeTypeDescription>> nodes;
        for(auto& nD : desc.m_nodeTypeDescription)
        {
            nodes.emplace_back(s::CreateNodeTypeDescriptionDirect(builder,
                                                                  nD.m_name.c_str(),
                                                                  nD.m_rtti.c_str()));
        }

        // Socket descriptions
        std::vector<flatbuffers::Offset<s::SocketTypeDescription>> sockets;
        for(auto& sD : desc.m_socketTypeDescription)
        {
            sockets.emplace_back(s::CreateSocketTypeDescriptionDirect(builder,
                                                                      sD.m_name.c_str(),
                                                                      sD.m_rtti.c_str()));
        }

        graphs.emplace_back(s::CreateGraphTypeDescriptionDirect(builder,
                                                                id.getName().c_str(),
                                                                id.getUniqueName().c_str(),
                                                                &nodes,
                                                                &sockets));
    }

    auto offset = s::CreateGetAllGraphTypeDescriptionsResponseDirect(builder, &graphs);
    builder.Finish(offset);

    // Set the response.
    response.setReady(ResponsePromise::Payload{makeBinaryBuffer(response.getAllocator(),
                                                                builder.GetBufferPointer(),
                                                                builder.GetSize()),
                                               "application/octet-stream"});
}