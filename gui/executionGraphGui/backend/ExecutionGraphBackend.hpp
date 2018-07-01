//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Sun Jan 14 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraphGui_backend_ExecutionGraphBackend_hpp
#define executionGraphGui_backend_ExecutionGraphBackend_hpp

#include <array>
#include <executionGraph/common/Identifier.hpp>
#include <executionGraph/graphs/ExecutionTreeInOut.hpp>
#include <rttr/type>
#include <string>
#include <unordered_set>
#include "backend/Backend.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    The execution graph backend.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class ExecutionGraphBackend final : public Backend
{
    RTTR_ENABLE()

public:
    using DefaultGraph = executionGraph::ExecutionTreeInOut<executionGraph::GeneralConfig<>>;
    using Id           = executionGraph::Id;

public:
    ExecutionGraphBackend()
        : Backend("ExecutionGraphBackend")
    {
    }
    ~ExecutionGraphBackend() override = default;

    //! Adding/removing graphs.
    //@{
    void addGraph(const Id& graphId,
                  const Id& graphType);
    void removeGraph(const Id& id);
    void removeGraphs();
    //@}

    //! Information about graphs.
    //@{
public:
    struct GraphDescription
    {
        std::unordered_set<std::string> m_nodeTypes;    //!< Type names of the available and creatable nodes on this graph.
        std::unordered_set<std::string> m_socketTypes;  //!< Type names of the available sockets.
    };

private:
    static const std::array<Id, 1> m_descriptionIds;                            //!< All IDs used for the graph description.
    static const std::unordered_map<Id, GraphDescription> m_graphDescriptions;  //! All graph descriptions.

public:
    //! Get all graph descriptions identified by its id.
    std::unordered_map<Id, GraphDescription> getGraphDescriptions() const { return m_graphDescriptions; }
    //@}

private:
    //! Map of normal graphs identified by its id.
    std::unordered_map<Id, DefaultGraph> m_graphs;
};

#endif
