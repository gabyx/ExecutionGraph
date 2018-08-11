// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Fri Jul 06 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGUI_backend_nodes_DummyNodeSerializer_hpp
#define executionGraphGUI_backend_nodes_DummyNodeSerializer_hpp

#include <executionGraph/nodes/LogicCommon.hpp>
#include <executionGraph/serialization/LogicNodeSerializer.hpp>
#include <executionGraphGUI/backend/nodes/DummyNode.hpp>
#include <flatbuffers/flatbuffers.h>

template<typename TConfig>
struct DummyNodeSerializer
{
    using DummyNodeType      = DummyNode<TConfig>;
    using VectorOffsetSocket = flatbuffers::Vector<flatbuffers::Offset<executionGraph::
                                                                           serialization::LogicSocket>>;
    //! Factory functor which creates the serialized write buffer
    //! for the DummyNode `node`.
    struct Writer
    {
        EXECGRAPH_DEFINE_CONFIG(TConfig);
        using Key = DummyNodeType;

        static std::pair<const uint8_t*, std::size_t>
        create(flatbuffers::FlatBufferBuilder& builder, const NodeBaseType& node)
        {
            return {nullptr, 0};
        }
    };

    //! Factory functor which creates the DummyNode from a serialized
    //! buffer `node`.
    struct Reader
    {
        EXECGRAPH_DEFINE_CONFIG(TConfig);
        using Key = DummyNodeType;

        static std::unique_ptr<NodeBaseType>
        create(executionGraph::NodeId nodeId,
               const std::string& name,
               const VectorOffsetSocket* inputSockets             = nullptr,
               const VectorOffsetSocket* outputSockets            = nullptr,
               const flatbuffers::Vector<uint8_t>* additionalData = nullptr)
        {
            return std::make_unique<DummyNodeType>(nodeId, name);
        }
    };
};

#endif