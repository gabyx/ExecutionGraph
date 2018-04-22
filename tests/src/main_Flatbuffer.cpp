//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Tue Jan 16 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#include <fstream>
#include <vector>
#include "TestFunctions.hpp"

#include <executionGraph/serialization/schemas/ExecutionGraph_generated.h>
#include <flatbuffers/flatbuffers.h>
#include "../files/testbuffer_generated.h"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif

MY_TEST(FlatBuffer, Test1)
{
    DEFINE_RANDOM_GENERATOR_FUNC(100);

    namespace s = executionGraph::serialization;

    auto makeLogicNodes = [&](auto& builder) {
        std::vector<flatbuffers::Offset<s::LogicNode>> nodes;
        int nNodes = int(rand() * 100.0);
        for(int i = 0; i < nNodes; ++i)
        {
            uint64_t id        = i;
            auto dummy         = builder.CreateString("DummyNode");
            auto dummyNodeData = s::CreateDummyNodeData(builder);
            s::LogicNodeBuilder lnBuilder(builder);
            lnBuilder.add_nodeId(id);
            lnBuilder.add_type(dummy);
            lnBuilder.add_data(dummyNodeData.Union());
            nodes.push_back(lnBuilder.Finish());
        }

        return builder.CreateVector(nodes);
    };

    flatbuffers::FlatBufferBuilder builder;
    {
        auto nodes = makeLogicNodes(builder);
        s::ExecutionGraphBuilder execGraphBuilder(builder);
        execGraphBuilder.add_nodes(nodes);
        auto execGraph = execGraphBuilder.Finish();
        builder.Finish(execGraph);
    }
    uint8_t* buf     = builder.GetBufferPointer();
    std::size_t size = builder.GetSize();

    std::ofstream file("myGraph.eg", std::ios_base::trunc | std::ios_base::binary | std::ios_base::in);
    file.write(reinterpret_cast<const char*>(buf), size);
    file.close();
}

MY_TEST(FlatBuffer, Test2)
{
    flatbuffers::FlatBufferBuilder builder;
    {
        testbuffer::Vec3 vec(1, 3, 4);
        auto testBuffer = testbuffer::CreateTestBufferDirect(builder, &vec, "asd", true);
        builder.Finish(testBuffer);
    }

    uint8_t* buf     = builder.GetBufferPointer();
    std::size_t size = builder.GetSize();
    std::vector<uint8_t> file(size);
    std::memcpy(file.data(), buf, size);

    // Reading back.
    auto readTestBuffer = testbuffer::GetTestBuffer(file.data());
    ASSERT_EQ(readTestBuffer->used(), true) << " Wupi, wrong serialization!";
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
