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

//! Add a new graph of type `graphType` to the backend.
Id ExecutionGraphBackend::addGraph(const Id& graphType)
{
    static_assert(m_graphTypeDescriptionIds.size() == 1, "You need to expand this functionality here!");
    Id newId;  // Generate new id

    if(graphType == m_graphTypeDescriptionIds[0])
    {
        auto graph = std::make_shared<Synchronized<DefaultGraph>>{};
        m_graphs.wlock()->emplace(std::make_pair(newId, graph));
    }
    else
    {
        EXECGRAPHGUI_THROW_BAD_REQUEST("Graph type: '{0}' not known!",
                                       graphType.toString());
    }
    return newId;
}

//! Remove a graph with id `graphId` from the backend.
void ExecutionGraphBackend::removeGraph(const Id& graphId)
{
    auto s = initRequest(graphId);

    //! @todo:
    //! - terminate any execution thread working on the graph

    // Holding a shared_ptr to the graphStatus is ok, since we are the only
    // function which is gona delete this!
    std::shared_ptr<GraphStatus> graphStatus;
    m_status.withWLock([&graphId](auto& status) {
        auto it = status.find(graphId);
        EXECGRAPH_ASSERT(it != status.cend(), "Programming error!");
        graphStatus = it->second;
        // Disable request handling for this graph
        // Important: No request may be handled anymore from now on!
        graphStatus->enableRequestHandling(false);
    });

    // Wait till all request are finished except this one
    auto result = graphStatus->waitUntilOnlySingleRequests();

    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(!result.second,
                                      "Time-out while waiting for all request to "
                                      "finish on graph '{0}'! Try later!",
                                      graphId.toString());

    // We are clear to delete all data structures for this graph
    clearGraphData(graphId;)
}

//! Remove all graphs from the backend.
void ExecutionGraphBackend::removeGraphs()
{
    // Make set of all graph ids.
    std::set<Id> ids;
    m_graphs.withRLock([&graphId](auto& graphs) {
        for(auto& kV : graphs)
        {
            ids.emplace{kV.first};
        }
    });

    // Remove all graphs
    for(auto& id : ids)
    {
        removeGraph(id);
    }
}

//! Remove a node with type `type` from the graph with id `graphId`.
void ExecutionGraphBackend::removeNode(const Id& graphId,
                                       NodeId nodeId)
{
    auto s = initRequest(graphId);

    GraphVariant graphVar;
    m_graphs.withRlock([&](auto& graphs){
        auto graphIt = graphs->find(graphId);
        EXECGRAPH_ASSERT(graphIt != graphs.end());
        graphVar = graphIt->second;
    }

    // Make a visitor to dispatch the "remove" over the variant...
    auto remove = [&](auto& graph) {
        // Remove the node (gets destroyed right here after this scope!)
        auto node = graph->wlock()->removeNode(nodeId);
        EXECGRAPHGUI_THROW_BAD_REQUEST_IF(node == nullptr,
                                          "Node with id '{0}' does not exist in graph with id '{1}'",
                                          nodeId,
                                          graphId.toString());
    };

    std::visit(remove, graphVar);
}

//! Initializes a request for graph id `graphId`.
//! @post There exists a status entry for this graph id.
Deferred ExecutionGraphBackend::initRequest(Id graphId)
{
    // Check if request handling for this graph is disabled
    m_status.withWLock([&graphId](auto& status) {
        auto it = status.find(graphId);

        EXECGRAPHGUI_THROW_BAD_REQUEST_IF(it != status.end(),
                                          "No status for graph id: '{0}', Graph doesn't exist!",
                                          graphId.toString());

        EXECGRAPHGUI_THROW_BAD_REQUEST_IF(!it->isRequestHandlingEnabled(),
                                          "Request is cancled since, request handling on "
                                          "the graph with id: '{0}' is disabled!",
                                          graphId.toString());

        // Request handling is enabled
        // Increment the request counter for this graph id
        it->incrementRequestCount();
    });

    // Return a deferred functor which decrements
    // the request count.
    return makeDeferred([graphId, this]() {
        auto locked = m_status.wlock();
        auto it     = locked->find(graphId);
        if(it != locked->end())
        {
            // We have a status
            it->decrementRequestCount();
        }
    });
}

//! Clears all data for this graph with id `graphId`.
void ExecutionGraphBackend::clearGraphData(Id graphId)
{
    auto nErased = m_graphs.wlock()->erase(graphId);
    EXECGRAPH_ASSERT(nErased == 0,
                     "No such graph with id: '{0}' removed!",
                     graphId.toString());

    nErased = m_status.wlock()->erase(graphId);
    EXECGRAPH_ASSERT(nErased == 0,
                     "No such graph status with id: '{0}' removed!",
                     graphId.toString());

    nErased = m_executor.wlock()->erase(graphId);
    EXECGRAPH_ASSERT(nErased == 0,
                     "No such graph status with id: '{0}' removed!",
                     graphId.toString());
}