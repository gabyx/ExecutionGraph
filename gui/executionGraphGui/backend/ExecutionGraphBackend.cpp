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

#include "backend/ExecutionGraphBackend.hpp"
#include "backend/ExecutionGraphBackendDefs.hpp"

using Id                   = ExecutionGraphBackend::Id;
using GraphTypeDescription = ExecutionGraphBackend::GraphTypeDescription;
using DefaultGraph         = ExecutionGraphBackend::DefaultGraph;
using DefaultGraphConfig   = typename DefaultGraph::Config;

namespace
{
    //! Return a graph description.
    template<typename Config>
    GraphTypeDescription makeGraphTypeDescription()
    {
        GraphTypeDescription description;
        //! All node types
        description.m_nodeTypeDescription = ExecutionGraphBackendDefs<DefaultGraphConfig>::NodeTypeDescriptions;
        //! All socket types
        description.m_socketTypeDescription = ExecutionGraphBackendDefs<DefaultGraphConfig>::SocketTypeDescriptions;

        return description;
    };
}  // namespace

//! Define all description Ids for the different graphs.
//! Default graph type has always this unique id!
const std::array<Id, 1> ExecutionGraphBackend::m_graphTypeDescriptionIds = {Id{"DefaultGraph",
                                                                               std::string("2992ebff-c950-4184-8876-5fe6ac029aa5")}};
//! Define all graph types in this Backend
//! Currently only the DefaultGraphConfig is added.
const std::unordered_map<Id, GraphTypeDescription>
    ExecutionGraphBackend::m_graphTypeDescription = {std::make_pair(m_graphTypeDescriptionIds[0],
                                                                    makeGraphTypeDescription<DefaultGraphConfig>())};

//! Add a graph of type `graphType` with id `graphId` to the backend.
void ExecutionGraphBackend::addGraph(const Id& graphId, const Id& graphType)
{
    static_assert(m_graphTypeDescriptionIds.size() == 1, "You need to expand this functionality here!");

    if(graphType == m_graphTypeDescriptionIds[0])
    {
        EXECGRAPH_THROW_EXCEPTION_IF(m_graphs.find(graphId) != m_graphs.end(),
                                     "Graph id '" << std::string(graphId) << "' already exists!");

        m_graphs.emplace(std::make_pair(graphId, DefaultGraph{}));
    }
    else
    {
        EXECGRAPH_THROW_EXCEPTION("Graph type '" << std::string(graphType)
                                                 << "' not available for adding!");
    }
}

//! Remove a graph with id `graphId` from the backend.
void ExecutionGraphBackend::removeGraph(const Id& id)
{
    auto nErased = m_graphs.erase(id);
    EXECGRAPH_THROW_EXCEPTION_IF(nErased == 0,
                                 "No such graph with id: '" << std::string(id) << "' removed!");
}

//! Remove all graphs from the backend.
void ExecutionGraphBackend::removeGraphs()
{
    m_graphs.clear();
}
