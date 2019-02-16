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

#include "executionGraphGui/backend/ExecutionGraphBackend.hpp"
#include "executionGraph/common/MetaVisit.hpp"
#include "executionGraphGui/backend/ExecutionGraphBackendDefs.hpp"
#include "executionGraphGui/common/Assert.hpp"
#include "executionGraphGui/common/Exception.hpp"
#include "executionGraphGui/common/RequestError.hpp"

using Id                   = ExecutionGraphBackend::Id;
using IdNamed              = ExecutionGraphBackend::IdNamed;
using GraphTypeDescription = ExecutionGraphBackend::GraphTypeDescription;
using NodeId               = ExecutionGraphBackend::NodeId;
using Deferred             = ExecutionGraphBackend::Deferred;

namespace e = executionGraph;

namespace
{
    using GraphConfigs = ExecutionGraphBackend::GraphConfigs;

    template<typename F>
    void forEachConfig(F&& func)
    {
        std::size_t idx = 0;
        meta::for_each(GraphConfigs{}, [&](auto graphConfig) { func(graphConfig, idx++); });
    }

    //! Define all description Ids for the different graphs.
    //! Default graph type has always this unique id!
    const auto& getGraphTypeDescriptionsIds()
    {
        // Build all ids.
        auto init = []() {
            std::array<IdNamed, ExecutionGraphBackend::nGraphTypes> ids;

            forEachConfig([&](auto graphConfig, auto idx) {
                using Config = decltype(graphConfig);
                ids[idx]     = ExecutionGraphBackendDefs<Config>::getId();
            });

            return ids;
        };

        static auto ids = init();
        return ids;
    }

    //! Define all description Ids to the index in the `GraphConfigs`.
    const auto& getGraphTypeDescriptionsToIndex()
    {
        auto init = []() {
            std::unordered_map<Id, std::size_t> map;
            std::size_t i = 0;
            for(auto& id : getGraphTypeDescriptionsIds())
            {
                map.emplace(id, i);
            }
            return map;
        };
        static auto m = init();
        return m;
    }

    //! Define all graph types in this Backend
    //! Currently only the DefaultGraphConfig is added.
    const auto& getGraphTypeDescriptions()
    {
        auto init = []() {
            auto& ids = getGraphTypeDescriptionsIds();
            std::unordered_map<Id, GraphTypeDescription> m;

            // Build the map.
            forEachConfig([&](auto graphConfig, auto idx) {
                using Config = decltype(graphConfig);
                m.emplace(ids[idx],
                          e::makeGraphTypeDescription<Config>(ids[idx],
                                                              ExecutionGraphBackendDefs<Config>::getNodeDescriptions(),
                                                              ExecutionGraphBackendDefs<Config>::getDescription()));
            });

            return m;
        };

        // All graph descriptions.
        static auto graphTypeDescription = init();
        return graphTypeDescription;
    }

}  // namespace

//! Get all graph description of supported graphs.
const std::unordered_map<Id, GraphTypeDescription>&
ExecutionGraphBackend::getGraphTypeDescriptions() const
{
    return ::getGraphTypeDescriptions();
}

//! Get all graph description of supported graphs.
const std::unordered_map<Id, std::size_t>&
ExecutionGraphBackend::getGraphTypeDescriptionsToIndex() const
{
    return ::getGraphTypeDescriptionsToIndex();
}

//! Save a graph to a file.
void ExecutionGraphBackend::saveGraph(const Id& graphId,
                                      std::path filePath,
                                      bool overwrite,
                                      BinaryBufferView visualization)
{
    auto deferred = initRequest(graphId);

    if(filePath.is_relative())
    {
        filePath = m_rootPath / filePath;
    }

    GraphVariant graphVar = getGraph(graphId);
    auto& id              = getGraphTypeDescriptionsIds()[graphVar.index()];
    auto& descs           = getGraphTypeDescriptions();

    auto save = [&](auto graph) {
        using GraphType = typename std::decay_t<decltype(*graph)>::DataType;
        using Config    = typename GraphType::Config;

        typename ExecutionGraphBackendDefs<Config>::NodeSerializer nodeS;
        typename ExecutionGraphBackendDefs<Config>::GraphSerializer graphS(nodeS);

        auto descIt = descs.find(id);
        EXECGRAPHGUI_ASSERT(descIt != descs.end(), "Graph Description not mapped (?)");

        graph->withRLock([&](auto& graph) { graphS.write(graph,
                                                         descIt->second,
                                                         filePath,
                                                         overwrite,
                                                         visualization); });
    };

    std::visit(save, graphVar);
}

//! Add a new graph of type `graphType` to the backend.
Id ExecutionGraphBackend::addGraph(const Id& graphType)
{
    const auto& idToIdx = getGraphTypeDescriptionsToIndex();
    auto it             = idToIdx.find(graphType);

    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(it == idToIdx.end(),
                                      "Graph type id: '{0}' not supported in backend!",
                                      graphType.toString());

    return meta::visit<GraphConfigs>(it->second, [&](auto graphConfig) {
        using Config = decltype(graphConfig);
        using Graph  = typename ExecutionGraphBackendDefs<Config>::Graph;
        Id newId;  // Generate new id
        auto graph       = std::make_shared<Synchronized<Graph>>();
        auto graphStatus = std::make_shared<GraphStatus>();
        m_graphs.wlock()->emplace(std::make_pair(newId, graph));
        m_status.wlock()->emplace(std::make_pair(newId, graphStatus));
        return newId;
    });
}

//! Remove a graph with id `graphId` from the backend.
void ExecutionGraphBackend::removeGraph(const Id& graphId)
{
    auto s = initRequest(graphId);

    //! @todo:
    //! - terminate any execution thread working on the graphGraphTypeDescription

    // Holding a shared_ptr to the graphStatus is ok, since we are the only
    // function which is gona delete this!
    std::shared_ptr<GraphStatus> graphStatus;
    m_status.withWLock([&graphId, &graphStatus](auto& status) {
        auto it = status.find(graphId);
        EXECGRAPHGUI_ASSERT(it != status.cend(),
                            "No status for graph id '{0}'",
                            graphId.toString());
        graphStatus = it->second;
    });

    // Disable request handling for this graph
    // Important: No request may be handled anymore from now on!
    graphStatus->setRequestHandlingEnabled(false);

    // Wait till all other requests are finished
    auto result = graphStatus->waitUntilOtherRequestsFinished();

    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(!result.second,
                                      "Time-out while waiting for all request to "
                                      "finish on graph '{0}'! Try later!",
                                      graphId.toString());

    // We are clear to delete all data structures for this graph
    clearGraphData(graphId);
}

//! Remove all graphs from the backend.
void ExecutionGraphBackend::removeGraphs()
{
    // Make set of all graph ids.
    std::unordered_set<Id> ids;
    m_graphs.withRLock([&ids](auto& graphs) {
        for(auto& kV : graphs)
        {
            ids.emplace(kV.first);
        }
    });

    // Remove all graphs
    for(auto& id : ids)
    {
        removeGraph(id);
    }
}

//! Remove a node with type `type` from the graph with id `graphId`.
void ExecutionGraphBackend::removeNode(const Id& graphId, NodeId nodeId)
{
    auto s = initRequest(graphId);

    GraphVariant graphVar = getGraph(graphId);

    // Make a visitor to dispatch the "remove" over the variant...
    auto remove = [&](auto& graph) {
        // Remove the node (gets destroyed right here after this scope!)
        auto node = graph->wlock()->removeNode(nodeId);
        EXECGRAPHGUI_THROW_BAD_REQUEST_IF(
            node == nullptr, "Node with id '{0}' does not exist in graph with id '{1}'", nodeId, graphId.toString());
    };

    std::visit(remove, graphVar);
}

//! Add a connection to the graph with id `graphId`.
void ExecutionGraphBackend::removeConnection(const Id& graphId,
                                             NodeId outNodeId,
                                             SocketIndex outSocketIdx,
                                             NodeId inNodeId,
                                             SocketIndex inSocketIdx,
                                             bool isWriteLink)
{
    auto deferred = initRequest(graphId);

    GraphVariant graphVar = getGraph(graphId);

    // Remark: Here somebody could potentially call `removeGraph` (other thread)
    // which waits till all requests on this graph are handled.

    // Make a visitor to dispatch the "remove" over the variant...
    auto remove = [&](auto& graph) {
        using GraphType = typename std::decay_t<decltype(*graph)>::DataType;
        using Config    = typename GraphType::Config;

        try
        {  // Locking start
            auto graphL = graph->wlock();
            if(isWriteLink)
            {
                graphL->removeWriteLink(outNodeId, outSocketIdx, inNodeId, inSocketIdx);
            }
            else
            {
                graphL->removeGetLink(inNodeId, inSocketIdx, &outNodeId, &outSocketIdx);
            }
        }  // Locking end
        catch(executionGraph::Exception& e)
        {
            EXECGRAPHGUI_THROW_BAD_REQUEST(
                std::string("Removing connection from output node id '{0}' [socket idx: '{1}'] ") +
                    (isWriteLink ? "<-- " : "--> ") + "input node id '{2}' [socket idx: '{3}' not successful!",
                outNodeId,
                outSocketIdx,
                inNodeId,
                inSocketIdx);
        }
    };

    std::visit(remove, graphVar);
}

//! Initializes a request for graph id `graphId`.
//! @post There exists a status entry for this graph id.
Deferred ExecutionGraphBackend::initRequest(Id graphId)
{
    // Check if request handling for this graph is disabled
    m_status.withWLock([&graphId](auto& stati) {
        auto it = stati.find(graphId);

        EXECGRAPHGUI_THROW_BAD_REQUEST_IF(
            it == stati.end(), "No status for graph id: '{0}', Graph doesn't exist!", graphId.toString());

        auto& status = it->second;
        EXECGRAPHGUI_THROW_BAD_REQUEST_IF(!status->isRequestHandlingEnabled(),
                                          "Request is cancled since, request handling on "
                                          "the graph with id: '{0}' is disabled!",
                                          graphId.toString());

        // Request handling is enabled
        // Increment the request counter for this graph id
        status->incrementRequestCount();
    });

    // Return a deferred functor which decrements
    // the request count.
    return executionGraph::makeDeferred([graphId, this]() {
        auto locked = m_status.wlock();
        auto it     = locked->find(graphId);
        if(it != locked->end())
        {
            // We have a status
            it->second->decrementRequestCount();
        }
    });
}

//! Clears all data for this graph with id `graphId`.
void ExecutionGraphBackend::clearGraphData(Id graphId)
{
    auto nErased = m_graphs.wlock()->erase(graphId);
    EXECGRAPH_ASSERT(nErased != 0, "No such graph with id: '{0}' removed!", graphId.toString());

    nErased = m_status.wlock()->erase(graphId);
    EXECGRAPH_ASSERT(nErased != 0, "No such graph status with id: '{0}' removed!", graphId.toString());

    // nErased = m_executor.wlock()->erase(graphId);
    // EXECGRAPH_ASSERT(nErased == 0,
    //                  "No such graph status with id: '{0}' removed!",
    //                  graphId.toString());
}

//! Get the graph corresponding to `graphId`.
ExecutionGraphBackend::GraphVariant ExecutionGraphBackend::getGraph(const Id& graphId)
{
    return m_graphs.withRLock([&](auto& graphs) {
        auto graphIt = graphs.find(graphId);
        EXECGRAPHGUI_THROW_BAD_REQUEST_IF(
            graphIt == graphs.cend(), "Graph id: '{0}' does not exist!", graphId.toString());
        return graphIt->second;
    });
}