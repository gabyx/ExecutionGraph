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
#define FLATBUFFERS_DEBUG_VERIFICATION_FAILURE 1
#include <executionGraph/common/Log.hpp>
#include <executionGraph/graphs/ExecutionTreeInOut.hpp>
#include <executionGraph/nodes/LogicNode.hpp>
#include <executionGraph/serialization/ExecutionGraphSerializer.hpp>
#include <executionGraph/serialization/FileMapper.hpp>
#include <executionGraph/serialization/LogicNodeSerializer.hpp>
#include <executionGraph/serialization/schemas/ExecutionGraph_generated.h>
#include <flatbuffers/flatbuffers.h>
#include <fstream>
#include <rttr/registration>
#include <vector>
#include "../files/testbuffer_generated.h"
#include "TestFunctions.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif

namespace s = executionGraph::serialization;

using Config    = executionGraph::GeneralConfig<>;
using GraphType = executionGraph::ExecutionTreeInOut<Config>;

class DummyNode final : public Config::NodeBaseType
{
    RTTR_ENABLE()
public:
    template<typename... Args>
    DummyNode(Args&&... args)
        : Config::NodeBaseType(std::forward<Args>(args)...){};
    ~DummyNode() = default;

private:
    void compute() override {}
    void reset() override {}
};

RTTR_REGISTRATION
{
    rttr::registration::class_<DummyNode>("DummyNode");
}

struct NodeSerializerWrite
{
    EXECGRAPH_TYPEDEF_CONFIG(Config);

    using Key = DummyNode;

    static flatbuffers::Offset<flatbuffers::Vector<uint8_t>>
    create(flatbuffers::FlatBufferBuilder& builder, const NodeBaseType& node)
    {
        return {};
    }
};
struct NodeSerializerRead
{
    EXECGRAPH_TYPEDEF_CONFIG(Config);

    using Key = DummyNode;

    static std::unique_ptr<NodeBaseType>
    create(executionGraph::NodeId id, const s::LogicNode& node)
    {
        return std::make_unique<DummyNode>(id);
    }
};

MY_TEST(FlatBuffer, Test1)
{
    unsigned int nNodes = 500;

    auto makeLogicNodes = [&](auto& builder) {
        std::vector<flatbuffers::Offset<s::LogicNode>> nodes;

        EXECGRAPH_LOG_TRACE("Serializing " << nNodes << " nodes!")
        for(int i = 0; i < nNodes; ++i)
        {
            uint64_t id = i;
            auto dummy  = builder.CreateString("DummyNode");

            // make some sepcific flexbuffer and add it as data()
            namespace t = test;
            flatbuffers::FlatBufferBuilder builderData;
            {
                std::vector<t::Vec3> vecs(3, t::Vec3(1, 3, 4));
                auto vecsOffsets = builderData.CreateVectorOfStructs(vecs.data(), vecs.size());
                auto testBuilder = t::TestBuilder(builderData);
                testBuilder.add_pos(vecsOffsets);
                auto test = testBuilder.Finish();

                builderData.Finish(test);
            }
            flatbuffers::Offset<flatbuffers::Vector<uint8_t>> data =
                builder.CreateVector(builderData.GetBufferPointer(), builderData.GetSize());

            s::LogicNodeBuilder lnBuilder(builder);
            lnBuilder.add_id(id);
            lnBuilder.add_type(dummy);
            lnBuilder.add_data(data);
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
        EXECGRAPH_LOG_TRACE("Read graph simple");
        executionGraph::FileMapper mapper("myGraph.eg");
        std::tie(buf, size) = mapper.getData();
        auto graph          = s::GetExecutionGraph(buf);
        ASSERT_TRUE(graph->nodes() != nullptr && graph->nodes()->size() == nNodes) << " Wupi, wrong serialization!";
    }

    {
        EXECGRAPH_LOG_TRACE("Read graph by Serializer");
        GraphType graph;
        using LogicNodeS = s::LogicNodeSerializer<Config,
                                                  meta::list<NodeSerializerWrite>,
                                                  meta::list<NodeSerializerRead>>;
        LogicNodeS nodeSerializer;
        s::ExecutionGraphSerializer<GraphType, LogicNodeS> serializer(graph, nodeSerializer);
        serializer.load("myGraph.eg");
    }

    std::filesystem::remove("myGraph.eg");
}

MY_TEST(FlatBuffer, Test2)
{
    unsigned int n = 100;
    namespace t    = test;

    flatbuffers::FlatBufferBuilder builder;
    std::vector<t::Vec3> vecs(n, t::Vec3(1, 3, 4));
    auto vecsOffsets = builder.CreateVectorOfStructs(vecs.data(), vecs.size());

    auto testBuilder = t::TestBuilder(builder);
    testBuilder.add_pos(vecsOffsets);
    auto test = testBuilder.Finish();
    builder.Finish(test);

    uint8_t* file    = builder.GetBufferPointer();
    std::size_t size = builder.GetSize();

    // Reading back.
    flatbuffers::Verifier v(file, size);
    ASSERT_TRUE(t::VerifyTestBuffer(v));
    auto rtest = t::GetTest(file);
    ASSERT_EQ(rtest->pos()->size(), n) << " Wupi, wrong serialization!";
    ASSERT_EQ((*rtest->pos())[n - 1]->z(), 4);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
