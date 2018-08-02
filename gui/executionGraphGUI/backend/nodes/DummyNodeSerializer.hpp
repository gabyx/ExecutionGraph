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
#include <executionGraphGUI/backend/nodes/DummyNode.hpp>
#include <flatbuffers/flatbuffers.h>

struct DummyNodeSerializer
{
    //! Factory functor which creates the serialized write buffer
    //! for the DummyNode `node`.
    struct Writer
    {
        EXECGRAPH_TYPEDEF_CONFIG(Config);

        using Key = DummyNodeType;

        static std::pair<const uint8_t*, std::size_t>
        create(flatbuffers::FlatBufferBuilder& builder, const NodeBaseType& node)
        {
            namespace t = test;
            std::vector<t::Vec3> vecs(3, t::Vec3(1, 3, 4));
            auto vecsOffsets = builder.CreateVectorOfStructs(vecs.data(), vecs.size());
            auto testBuilder = t::TestBuilder(builder);
            testBuilder.add_pos(vecsOffsets);
            auto test = testBuilder.Finish();

            builder.Finish(test);
            return {builder.GetBufferPointer(), builder.GetSize()};
        }
    };
    //! Factory functor which creates the DummyNode from a serialized
    //! buffer `node`.
    struct Reader
    {
        EXECGRAPH_TYPEDEF_CONFIG(Config);

        using Key = DummyNodeType;

        static std::unique_ptr<NodeBaseType>
        create(const executionGraph::NodeId& nodeId,
               flatbuffers::Vector<uint8_t>* additionalData = nullptr)
        {
            return std::make_unique<DummyNodeType>(nodeId);
        }
    };
};

#endif