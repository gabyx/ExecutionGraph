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
#include "executionGraph/common/Identifier.hpp"
#include "executionGraph/serialization/NodeTypeDescription.hpp"
#include "executionGraph/serialization/SocketTypeDescription.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    Data structure to store the description for a graph.

    @date Fri Aug 10 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/

namespace executionGraph
{
    class GraphTypeDescription
    {
    public:
        GraphTypeDescription(const IdName& graphId, std::vector<SocketTypeDescription> socketTypeDescription)
            : m_graphId(graphId), m_socketTypeDescription(std::move(socketTypeDescription))
        {}

    public:
        //! Get the RTTI of the socket with type `type`.
        const std::string& getSocketRTTI(IndexType type)
        {
            EXECGRAPH_ASSERT(type < m_socketTypeDescription.size());
            return m_socketTypeDescription[type].m_rtti;
        }

    public:
        const IdNamed m_graphId;                                           //!< The id of this graph type.
        const std::vector<SocketTypeDescription> m_socketTypeDescription;  //!< Type names of the available sockets (in order with the `Config::SocketTypes`)

        std::vector<NodeTypeDescription> m_nodeTypeDescription;  //!< Type names of the available and creatable nodes on this graph.
    };

    //! Return a graph description.
    template<typename Config>
    GraphTypeDescription makeGraphTypeDescription(const IdNamed& graphId,
                                                  std::vector<NodeTypeDescription> nodeTypeDescription = {})
    {
        GraphTypeDescription description(graphId, getSocketDescriptions<Config>());

        //! All node types
        description.m_nodeTypeDescription = std::move(nodeTypeDescription);

        return description;
    };

}  // namespace executionGraph
#endif