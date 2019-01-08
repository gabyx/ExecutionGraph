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

#include "executionGraphGui/backend/requestHandlers/FileBrowserHandler.hpp"
#include <executionGraph/serialization/GraphTypeDescriptionSerializer.hpp>
#include "executionGraphGui/backend/ExecutionGraphBackend.hpp"
#include "executionGraphGui/common/AllocatorProxyFlatBuffer.hpp"
#include "executionGraphGui/common/Exception.hpp"
#include "executionGraphGui/common/Loggers.hpp"

using namespace executionGraph;
namespace s  = executionGraph::serialization;
namespace sG = executionGraphGui::serialization;

//! Init the function mapping.
FunctionMap<FileBrowserHandler::Function> FileBrowserHandler::initFunctionMap()
{
    using Entry = typename FunctionMap<Function>::Entry;
    auto r      = {Entry{targetBase / "files",
                    Function{&FileBrowserHandler::handleGetAllGraphTypeDescriptions}}};
    return {r};
}

//! Static handler map: request to handler function mapping.
const FunctionMap<FileBrowserHandler::Function> FileBrowserHandler::m_functionMap = FileBrowserHandler::initFunctionMap();

//! Konstructor.
FileBrowserHandler::FileBrowserHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                                       const IdNamed& id)
    : BackendRequestHandler(id)
{
}

//! Get the request types for which this handler is registered.
const std::unordered_set<std::string>& FileBrowserHandler::getRequestTypes() const
{
    return m_functionMap.m_keys;
}

//! Handle the request.
void FileBrowserHandler::handleRequest(const Request& request,
                                       ResponsePromise& response)
{
    EXECGRAPHGUI_BACKENDLOG_INFO("FileBrowserHandler::handleRequest");

    // Dispatch to the correct function
    auto it = m_functionMap.m_map.find(request.getTarget().string());
    if(it != m_functionMap.m_map.end())
    {
        it->second(*this, request, response);
    }
}

//! Handle the operation of getting all graph type descriptions.
void FileBrowserHandler::handleFileBrowser(const Request& request,
                                           ResponsePromise& response)
{
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(request.getPayload() != std::nullopt,
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
    response.setReady(ResponsePromise::Payload{releaseIntoBinaryBuffer(std::move(allocator),
                                                                       builder),
                                               "application/octet-stream"});
}