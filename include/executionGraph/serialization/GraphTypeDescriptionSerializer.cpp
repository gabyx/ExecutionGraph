// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Aug 11 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#include "executionGraph/serialization/GraphTypeDescriptionSerializer.hpp"
#include <vector>
#include "executionGraph/serialization/GraphTypeDescription.hpp"

namespace s = executionGraph::serialization;

namespace executionGraph
{
    flatbuffers::Offset<s::GraphTypeDescription> GraphTypeDescriptionSerializer::write(flatbuffers::FlatBufferBuilder& builder,
                                                                                       const GraphTypeDescription& graphDescription)
    {
        // Node descriptions
        std::vector<flatbuffers::Offset<s::NodeTypeDescription>> nodes;
        for(auto& nD : desc.m_nodeTypeDescription)
        {
            nodes.emplace_back(s::CreateNodeTypeDescriptionDirect(builder,
                                                                  nD.m_name.c_str(),
                                                                  nD.m_rtti.c_str()));
        }

        // Socket descriptions
        std::vector<flatbuffers::Offset<s::SocketTypeDescription>> sockets;
        for(auto& sD : desc.m_socketTypeDescription)
        {
            sockets.emplace_back(s::CreateSocketTypeDescriptionDirect(builder,
                                                                      sD.m_name.c_str(),
                                                                      sD.m_rtti.c_str()));
        }

    return s::CreateGraphTypeDescriptionDirect(builder,desc.m_id.getShortName().c_str(),
                                               desc.m_id.toString().c_str(),
                                               &nodes,
                                               &sockets));
    }
}  // namespace executionGraph