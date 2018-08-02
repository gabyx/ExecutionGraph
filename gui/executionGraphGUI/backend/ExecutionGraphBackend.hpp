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

#ifndef executionGraphGUI_backend_ExecutionGraphBackend_hpp
#define executionGraphGUI_backend_ExecutionGraphBackend_hpp

#include <array>
#include <executionGraph/common/Identifier.hpp>
#include <executionGraph/graphs/ExecutionTreeInOut.hpp>
#include <rttr/type>
#include <string>
#include <vector>
#include "executionGraphGUI/backend/Backend.hpp"
#include "executionGraphGUI/backend/nodes/NodeTypeDescription.hpp"
#include "executionGraphGUI/backend/nodes/SocketTypeDescription.hpp"

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
    using IdNamed      = executionGraph::IdNamed;

    using NodeId = executionGraph::NodeId;

public:
    ExecutionGraphBackend()
        : Backend(IdNamed("ExecutionGraphBackend"))
    {
    }
    ~ExecutionGraphBackend() override = default;

    //! Adding/removing graphs.
    //@{
    void addGraph(const Id& graphId,
                  const Id& graphType);
    void removeGraph(const Id& graphId);
    void removeGraphs();
    //@}

    //! Adding/removing nodes.
    //@{
    void addNode(const Id& graphId,
                 const std::string& type);
    void removeNode(const Id& graphId, NodeId id);
    //@}

    //! Information about graphs.
    //@{
public:
    struct GraphTypeDescription
    {
        IdNamed m_id;                                                //!< The id of this graph type.
        std::vector<NodeTypeDescription> m_nodeTypeDescription;      //!< Type names of the available and creatable nodes on this graph.
        std::vector<SocketTypeDescription> m_socketTypeDescription;  //!< Type names of the available sockets.
    };

private:
    static const std::array<IdNamed, 1> m_graphTypeDescriptionIds;                     //!< All IDs used for the graph description.
    static const std::unordered_map<Id, GraphTypeDescription> m_graphTypeDescription;  //!< All graph descriptions.

public:
    //! Get all graph descriptions identified by its id.
    std::unordered_map<Id, GraphTypeDescription> getGraphTypeDescriptions() const { return m_graphTypeDescription; }
    //@}

private:
    using GraphVariant = std::variant<DefaultGraph>;
    //! Map of normal graphs identified by its id.
    std::unordered_map<Id, GraphVariant> m_graphs;
};

#endif
