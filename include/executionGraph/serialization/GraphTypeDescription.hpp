// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Fri Aug 10 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraph_serialization_GraphTypeDescription_hpp
#define executionGraph_serialization_GraphTypeDescription_hpp

#include <string>
#include <vector>
#include "executionGraph/common/Assert.hpp"
#include "executionGraph/common/Identifier.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraph/serialization/NodeTypeDescription.hpp"
#include "executionGraph/serialization/SocketTypeDescription.hpp"

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
        Data structure to store the description for a graph.

        @date Fri Aug 10 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
     */
    /* ---------------------------------------------------------------------------------------*/
    class GraphTypeDescription
    {
    public:
        using SocketTypeDescriptionList = std::vector<SocketTypeDescription>;
        using NodeTypeDescriptionList   = std::vector<NodeTypeDescription>;

    public:
        GraphTypeDescription(const IdNamed& graphId, SocketTypeDescriptionList socketTypeDescription)
            : m_graphId(graphId), m_socketTypeDescription(std::move(socketTypeDescription))
        {}

    public:
        //! Get the RTTI of the socket with type `type`.
        const std::string& getSocketRTTI(IndexType type)
        {
            EXECGRAPH_ASSERT(type < m_socketTypeDescription.size(),
                             "Index type: '{0}' out of range!",
                             type);
            return m_socketTypeDescription[type].m_type;
        }

        //! Get the graph id.
        const IdNamed& getGraphId() const { return m_graphId; }
        //! Get the socket type descriptions.
        const SocketTypeDescriptionList& getSocketTypeDescriptions() const { return m_socketTypeDescription; }
        //! Get the node type descriptions.
        const NodeTypeDescriptionList& getNodeTypeDescriptions() const { return m_nodeTypeDescription; }
        NodeTypeDescriptionList& getNodeTypeDescriptions() { return m_nodeTypeDescription; }

    public:
        const IdNamed m_graphId;                                  //!< The id of this graph type.
        const SocketTypeDescriptionList m_socketTypeDescription;  //!< Type names of the available sockets (in order with the `Config::SocketTypes`)
        NodeTypeDescriptionList m_nodeTypeDescription;            //!< Type names of the available and creatable nodes on this graph.
    };

    //! Return a graph description from a graph id `graphId` and
    //! its node type descriptions `nodeTypeDescriptions`.
    template<typename Config>
    GraphTypeDescription makeGraphTypeDescription(const IdNamed& graphId,
                                                  std::vector<NodeTypeDescription> nodeTypeDescription = {})
    {
        GraphTypeDescription description(graphId, getSocketDescriptions<Config>());

        //! All node types
        description.getNodeTypeDescriptions() = std::move(nodeTypeDescription);

        return description;
    }

}  // namespace executionGraph
#endif