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
#include <fstream>
#include <vector>
#include <flatbuffers/flatbuffers.h>
#include <executionGraph/common/Log.hpp>
#include <executionGraph/graphs/ExecutionTree.hpp>
#include <executionGraph/nodes/LogicNode.hpp>
#include <executionGraph/serialization/ExecutionGraphSerializer.hpp>
#include <executionGraph/serialization/FileMapper.hpp>
#include <executionGraph/serialization/LogicNodeSerializer.hpp>
#include <executionGraph/serialization/schemas/cpp/ExecutionGraph_generated.h>
#include "../files/testbuffer_generated.h"
#include "DummyNode.hpp"
#include "GraphGenerator.hpp"
#include "TestFunctions.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif

MY_TEST(FlatBuffer, Test1)
{
    unsigned int n = 100;
    namespace t    = test;

    flatbuffers::FlatBufferBuilder builder(2500);
    std::vector<t::Vec3> vecs(n, t::Vec3(1, 3, 4));
    auto vecsOffsets  = builder.CreateVectorOfStructs(vecs.data(), vecs.size());
    auto vecsOffsets2 = builder.CreateVectorOfStructs(vecs.data(), vecs.size());

    auto testBuilder = t::TestBuilder(builder);
    testBuilder.add_pos(vecsOffsets);
    testBuilder.add_pos2(vecsOffsets2);

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

namespace s = executionGraph::serialization;

using Config        = executionGraph::GeneralConfig<>;
using GraphType     = executionGraph::ExecutionTree<Config>;
using DummyNodeType = DummyNode<Config>;
static const DummyNodeType::AutoRegisterRTTR autoRegisterRTTR;

struct DummyNodeSerializer
{
    //! Factory functor which creates the serialized write buffer
    //! for the DummyNode `node`.
    struct Writer
    {
        EXECGRAPH_DEFINE_CONFIG(Config);

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
        EXECGRAPH_DEFINE_CONFIG(Config);

        using Key = DummyNodeType;

        static std::unique_ptr<NodeBaseType>
        create(executionGraph::NodeId nodeId,
               const std::string& name,
               const flatbuffers::Vector<flatbuffers::Offset<s::LogicSocket>>* inputSockets  = nullptr,
               const flatbuffers::Vector<flatbuffers::Offset<s::LogicSocket>>* outputSockets = nullptr,
               const flatbuffers::Vector<uint8_t>* additionalData                            = nullptr)
        {
            return std::make_unique<DummyNodeType>(nodeId, name);
        }
    };
};

MY_TEST(FlatBuffer, Test2)
{
    using namespace executionGraph;
    unsigned int nNodes = 500;

    auto makeLogicNodes = [&](auto& builder) {
        std::vector<flatbuffers::Offset<s::LogicNode>> nodes;

        EXECGRAPH_LOG_TRACE("Serializing " << nNodes << " nodes!")
        for(int i = 0; i < nNodes; ++i)
        {
            uint64_t id = i;
            // DummyNodeType a(1, "asd");
            // int aa = DummyNodeType::autoRegisterRTTR.m_a + 1;
            auto dummy = builder.CreateString(rttr::type::get<DummyNodeType>().get_name().to_string());

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
        std::tie(buf, size) = mapper.data();
        auto graph          = s::GetExecutionGraph(buf);
        ASSERT_TRUE(graph->nodes() != nullptr && graph->nodes()->size() == nNodes) << " Wupi, wrong serialization!";
    }

    {
        EXECGRAPH_LOG_TRACE("Read graph by Serializer");

        using LogicNodeS = LogicNodeSerializer<Config,
                                               meta::list<DummyNodeSerializer>>;
        LogicNodeS nodeSerializer;
        ExecutionGraphSerializer<GraphType, LogicNodeS> serializer(nodeSerializer);
        auto execGraph = serializer.read("myGraph.eg");

        EXECGRAPH_LOG_TRACE("Write graph by Serializer");
        serializer.write(*execGraph,
                         makeGraphTypeDescription<Config>(IdNamed{"Graph1"}),
                         "myGraph-out.eg",
                         true);
    }

    std::filesystem::remove("myGraph.eg");
}

MY_TEST(FlatBuffer, Test3)
{
    using namespace executionGraph;

    EXECGRAPH_LOG_TRACE("Build graph");
    auto execGraph   = createRandomTree<GraphType, DummyNodeType>(3, 123456);
    using LogicNodeS = LogicNodeSerializer<Config,
                                           meta::list<DummyNodeSerializer>>;
    LogicNodeS nodeSerializer;
    ExecutionGraphSerializer<GraphType, LogicNodeS> serializer(nodeSerializer);
    EXECGRAPH_LOG_TRACE("Write graph by Serializer");
    serializer.write(*execGraph,
                     makeGraphTypeDescription<Config>(IdNamed{"Graph1"}),
                     "myGraph.eg",
                     true);
    auto graphRead = serializer.read("myGraph.eg");

    std::filesystem::remove("myGraph.eg");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
