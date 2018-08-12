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

#include "executionGraphGUI/backend/ExecutionGraphBackend.hpp"
#include "executionGraphGUI/backend/ExecutionGraphBackendDefs.hpp"
#include "executionGraphGUI/common/Exception.hpp"
#include "executionGraphGUI/common/RequestError.hpp"

using Id                   = ExecutionGraphBackend::Id;
using IdNamed              = ExecutionGraphBackend::IdNamed;
using GraphTypeDescription = ExecutionGraphBackend::GraphTypeDescription;
using DefaultGraph         = ExecutionGraphBackend::DefaultGraph;
using DefaultGraphConfig   = typename DefaultGraph::Config;
using NodeId               = ExecutionGraphBackend::NodeId;

namespace e = executionGraph;

//! Define all description Ids for the different graphs.
//! Default graph type has always this unique id!
const std::array<IdNamed, 1> ExecutionGraphBackend::m_graphTypeDescriptionIds = {IdNamed{"DefaultGraph",
                                                                                         std::string("2992ebff-c950-4184-8876-5fe6ac029aa5")}};
//! Define all graph types in this Backend
//! Currently only the DefaultGraphConfig is added.
const std::unordered_map<Id, e::GraphTypeDescription>
    ExecutionGraphBackend::m_graphTypeDescription = {std::make_pair(m_graphTypeDescriptionIds[0],
                                                                    e::makeGraphTypeDescription<DefaultGraphConfig>(m_graphTypeDescriptionIds[0],
                                                                                                                    ExecutionGraphBackendDefs<DefaultGraphConfig>::getNodeDescriptions()))};

//! Add a graph of type `graphType` with id `graphId` to the backend.
void ExecutionGraphBackend::addGraph(const Id& graphId, const Id& graphType)
{
    static_assert(m_graphTypeDescriptionIds.size() == 1, "You need to expand this functionality here!");

    if(graphType == m_graphTypeDescriptionIds[0])
    {
        EXECGRAPHGUI_THROW_BAD_REQUEST_IF(m_graphs.find(graphId) != m_graphs.end(),
                                          "Graph id '{0}' already exists!",
                                          graphId.toString());

        m_graphs.emplace(std::make_pair(graphId, DefaultGraph{}));
    }
    else
    {
        EXECGRAPHGUI_THROW_BAD_REQUEST("Graph type: '{0}' not known!",
                                       graphType.toString());
    }
}

//! Remove a graph with id `graphId` from the backend.
void ExecutionGraphBackend::removeGraph(const Id& graphId)
{
    auto nErased = m_graphs.erase(graphId);
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(nErased == 0,
                                      "No such graph id: '{0}' removed!",
                                      graphId.toString());
}

//! Remove all graphs from the backend.
void ExecutionGraphBackend::removeGraphs()
{
    m_graphs.clear();
}

//! Remove a node with type `type` from the graph with id `graphId`.
void ExecutionGraphBackend::removeNode(const Id& graphId,
                                       NodeId nodeId)
{
    auto graphIt = m_graphs.find(graphId);
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(graphIt == m_graphs.end(),
                                      "Graph id: '{0}' does not exist!",
                                      graphId.toString());
    GraphVariant& graphVar = graphIt->second;

    // Make a visitor to dispatch the "add" over the variant...
    auto remove = [&](auto& graph) {
        using GraphType = std::remove_cv_t<std::remove_reference_t<decltype(graph)>>;
        using Config    = typename GraphType::Config;

        // Remove the node (gets destroyed right here after this scope!)
        auto node = graph.removeNode(nodeId);

        EXECGRAPHGUI_THROW_BAD_REQUEST_IF(node == nullptr,
                                          "Node with id '{0}' does not exist in graph with id '{1}'",
                                          nodeId,
                                          graphId.toString());
    };

    std::visit(remove, graphVar);
}