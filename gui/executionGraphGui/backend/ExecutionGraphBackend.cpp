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

using Id                 = ExecutionGraphBackend::Id;
using GraphDescription   = ExecutionGraphBackend::GraphDescription;
using DefaultGraph       = ExecutionGraphBackend::DefaultGraph;
using DefaultGraphConfig = typename DefaultGraph::Config;

namespace
{
    //! Return a set of rtti strings.
    template<typename T>
    struct makeTypeSet;

    //! Spezialization for `meta::list<...>`.
    template<typename... Args>
    struct makeTypeSet<meta::list<Args...>>
    {
        auto operator()()
        {
            return std::unordered_set<std::string>{{rttr::type::get<Args>().get_name().to_string()...}};
        }
    };

    //! Return a graph description.
    template<typename Config>
    struct makeGraphDescription;

    //! Spezialization for `DefaultGraphConfig`.
    template<>
    struct makeGraphDescription<DefaultGraphConfig>
    {
        auto operator()()
        {
            GraphDescription description;
            // All node types
            //!@todo

            // All socket types
            description.m_socketTypes = makeTypeSet<typename DefaultGraphConfig::SocketTypes>{}();

            return description;
        }
    };
}  // namespace

const std::array<Id, 1> ExecutionGraphBackend::m_descriptionIds = {Id{"DefaultGraph",
                                                                      std::string("2992ebff-c950-4184-8876-5fe6ac029aa5")}};

const std::unordered_map<Id, GraphDescription>
    ExecutionGraphBackend::m_graphDescriptions = {std::make_pair(m_descriptionIds[0],
                                                                 makeGraphDescription<DefaultGraphConfig>{}())};

//! Add a graph of type `graphType` with id `graphId` to the backend.
void ExecutionGraphBackend::addGraph(const Id& graphId,
                                     const Id& graphType)
{
    static_assert(m_descriptionIds.size() == 1, "You need to expand this functionality here!");

    if(graphType == m_descriptionIds[0])
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
