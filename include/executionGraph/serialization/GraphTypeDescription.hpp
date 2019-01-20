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
        GraphTypeDescription(const IdNamed& graphId,
                             SocketTypeDescriptionList socketTypeDescription,
                             NodeTypeDescriptionList nodeTypeDescription = {},
                             std::string description                     = "no description")
            : m_graphId(graphId)
            , m_socketTypeDescription(std::move(socketTypeDescription))
            , m_nodeTypeDescription(std::move(nodeTypeDescription))
            , m_description(std::move(description))
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

        const IdNamed& getGraphId() const { return m_graphId; }
        const SocketTypeDescriptionList& getSocketTypeDescriptions() const { return m_socketTypeDescription; }
        const NodeTypeDescriptionList& getNodeTypeDescriptions() const { return m_nodeTypeDescription; }
        const std::string& getDescription() const { return m_description; }

    public:
        const IdNamed m_graphId;  //!< The id of this graph type.
        //! Type names of the available sockets (in order with the `Config::SocketTypes`)
        const SocketTypeDescriptionList m_socketTypeDescription;
        //! Type names of the available and creatable nodes on this graph.
        NodeTypeDescriptionList m_nodeTypeDescription;
        //! Some description of this graph type.
        const std::string m_description;
    };

    //! Return a graph description from a graph id `graphId` and
    //! its node type descriptions `nodeTypeDescriptions`.
    template<typename Config, typename T>
    GraphTypeDescription makeGraphTypeDescription(const IdNamed& graphId,
                                                  const GraphTypeDescription::NodeTypeDescriptionList& nodeTypeDescription,
                                                  T&& description)
    {
        return GraphTypeDescription(graphId,
                                    getSocketDescriptions<Config>(),
                                    nodeTypeDescription,
                                    std::forward<T>(description));
    }

}  // namespace executionGraph
#endif