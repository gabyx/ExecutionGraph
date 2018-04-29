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

#include <executionGraph/common/Log.hpp>
#include <executionGraph/graphs/ExecutionTreeInOut.hpp>
#include <executionGraph/serialization/ExecutionGraphSerializer.hpp>
#include <executionGraph/serialization/FileMapper.hpp>
#include <executionGraph/serialization/schemas/ExecutionGraph_generated.h>
#include <flatbuffers/flatbuffers.h>
#include <fstream>
#include <vector>
#include "../files/testbuffer_generated.h"
#include "TestFunctions.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif

MY_TEST(FlatBuffer, Test1)
{
    namespace s = executionGraph::serialization;

    unsigned int nNodes = 500;

    auto makeLogicNodes = [&](auto& builder) {
        std::vector<flatbuffers::Offset<s::LogicNode>> nodes;

        EXECGRAPH_LOG_TRACE("Serializing " << nNodes << " nodes!")
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
        EXECGRAPH_LOG_TRACE("Serializing done!");
        return builder.CreateVector(nodes);
    };

    flatbuffers::FlatBufferBuilder builder;
    {
        auto nodes = makeLogicNodes(builder);
        s::ExecutionGraphBuilder execGraphBuilder(builder);
        execGraphBuilder.add_nodes(nodes);
        auto execGraph = execGraphBuilder.Finish();
        FinishExecutionGraphBuffer(builder, execGraph);
    }
    const uint8_t* buf = builder.GetBufferPointer();
    std::size_t size   = builder.GetSize();

    EXECGRAPH_LOG_TRACE("Write file");
    std::ofstream file("myGraph.eg", std::ios_base::trunc | std::ios_base::binary | std::ios_base::in);
    file.write(reinterpret_cast<const char*>(buf), size);
    file.close();
    EXECGRAPH_LOG_TRACE("Write file done");

    {
        EXECGRAPH_LOG_TRACE("Read graph 1");
        executionGraph::FileMapper mapper("myGraph.eg");
        std::tie(buf, size) = mapper.getData();
        auto graph          = s::GetExecutionGraph(buf);
        ASSERT_TRUE(graph->nodes() != nullptr && graph->nodes()->size() == nNodes) << " Wupi, wrong serialization!";
    }

    {
        EXECGRAPH_LOG_TRACE("Read graph 2");
        using Config    = executionGraph::GeneralConfig<>;
        using GraphType = executionGraph::ExecutionTreeInOut<Config>;
        GraphType graph;
        s::ExecutionGraphSerializer<GraphType> serializer(graph);
        serializer.load("myGraph.eg");
    }

    std::filesystem::remove("myGraph.eg");
}

MY_TEST(FlatBuffer, Test2)
{
    unsigned int n = 1;
    namespace t    = test;

    flatbuffers::FlatBufferBuilder builder;
    std::vector<t::Vec3> vecs(n, t::Vec3(1, 3, 4));
    auto vecsOffsets = builder.CreateVectorOfStructs(vecs.data(), vecs.size());
    auto testB       = t::TestBuilder(builder);
    testB.add_pos(vecsOffsets);
    auto test = testB.Finish();
    builder.Finish(test);

    uint8_t* buf     = builder.GetBufferPointer();
    std::size_t size = builder.GetSize();
    std::vector<uint8_t> file(size);
    std::memcpy(file.data(), buf, size);

    // Reading back.
    flatbuffers::Verifier v(file.data(), size);
    //ASSERT_TRUE(t::VerifyTestBuffer(v));
    auto rtest = t::GetTest(file.data());
    ASSERT_EQ(rtest->pos()->size(), n) << " Wupi, wrong serialization!";
    ASSERT_EQ((*rtest->pos())[n - 1]->z(), 4);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
