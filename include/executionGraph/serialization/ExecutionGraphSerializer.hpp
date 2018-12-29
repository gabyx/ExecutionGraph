// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Apr 22 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraph_serialization_ExecutionGraphSerializer_hpp
#define executionGraph_serialization_ExecutionGraphSerializer_hpp

#include "executionGraph/common/Assert.hpp"
#include "executionGraph/common/Exception.hpp"
#include "executionGraph/common/Log.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/serialization/FileMapper.hpp"
#include "executionGraph/serialization/GraphTypeDescriptionSerializer.hpp"
#include "executionGraph/serialization/schemas/cpp/ExecutionGraph_generated.h"

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
        Serializer to store and read an execution graph.

        @date Sat Apr 28 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
        */
    /* ---------------------------------------------------------------------------------------*/

    template<typename TGraphType, typename TLogicNodeSerializer>
    class ExecutionGraphSerializer
    {
    public:
        using GraphType = TGraphType;
        using TConfig   = typename GraphType::Config;
        EXECGRAPH_DEFINE_CONFIG(TConfig);
        using LogicNodeSerializer = TLogicNodeSerializer;

        ExecutionGraphSerializer(LogicNodeSerializer& nodeSerializer)
            : m_nodeSerializer(nodeSerializer)
        {}
        ~ExecutionGraphSerializer() = default;

    public:
        //! Read an execution graph from a file `filePath`.
        std::unique_ptr<GraphType>
        read(const std::path& filePath) noexcept(false)
        {
            namespace s = serialization;
            using namespace s;

            m_filePath = filePath;
            // Memory mapping the file
            FileMapper mapper(m_filePath);
            const uint8_t* buffer = nullptr;
            std::size_t size;
            std::tie(buffer, size) = mapper.data();
            EXECGRAPH_ASSERT(buffer != nullptr,
                             "FileMapper returned nullptr for file '{0}'",
                             m_filePath);

            // Deserialize
            EXECGRAPH_THROW_IF(!ExecutionGraphBufferHasIdentifier(buffer),
                               "File identifier in '{0}' not found!",
                               filePath);

            flatbuffers::Verifier verifier(buffer, size, 64, 1000000000);
            EXECGRAPH_THROW_IF(!VerifyExecutionGraphBuffer(verifier),
                               "Buffer in '{0}' could not be verified!",
                               filePath);

            auto graph = GetExecutionGraph(buffer);

            EXECGRAPH_THROW_IF(graph == nullptr,
                               "Deserialization from '{0}' is invalid!",
                               filePath);

            auto execGraph = std::make_unique<GraphType>();
            readGraph(*execGraph, *graph);
            return std::move(execGraph);
        }

        //! Write an execution graph to the file `filePath`.
        void write(const GraphType& execGraph,
                   const GraphTypeDescription& graphDescription,
                   const std::path& filePath,
                   bool bOverwrite = false) noexcept(false)
        {
            flatbuffers::FlatBufferBuilder builder;
            auto graphOffset = writeGraph(builder, execGraph, graphDescription);
            FinishExecutionGraphBuffer(builder, graphOffset);

            std::ofstream file;
            EXECGRAPH_THROW_IF(!bOverwrite && std::filesystem::exists(filePath),
                               "File '{0}' already exists!",
                               filePath);

            file.open(filePath.string(), std::ios_base::trunc | std::ios_base::binary | std::ios_base::in);
            file.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
            file.close();
        }

    private:
        //! Serialize a graph `graph` and return the offsets.
        flatbuffers::Offset<serialization::ExecutionGraph>
        writeGraph(flatbuffers::FlatBufferBuilder& builder,
                   const GraphType& execGraph,
                   const GraphTypeDescription& graphDescription) const
        {
            namespace s = serialization;
            using namespace s;

            flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<s::LogicNode>>> nodesOffset;
            flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<s::ExecutionGraphNodeProperties>>> nodePropertiesOffset;

            auto graphDescOff                           = GraphTypeDescriptionSerializer::write(builder, graphDescription);
            std::tie(nodesOffset, nodePropertiesOffset) = writeNodes(builder, execGraph);
            auto linksOffset                            = writeLinks(builder, execGraph);

            ExecutionGraphBuilder graphBuilder(builder);
            graphBuilder.add_graphDescription(graphDescOff);
            graphBuilder.add_nodes(nodesOffset);
            graphBuilder.add_nodeProperties(nodePropertiesOffset);
            graphBuilder.add_links(linksOffset);
            return graphBuilder.Finish();
        }

        //! Serialize all nodes of the graph `execGraph` and return the offsets.
        auto writeNodes(flatbuffers::FlatBufferBuilder& builder, const GraphType& execGraph) const
        {
            namespace s = serialization;
            using namespace s;

            std::vector<flatbuffers::Offset<s::ExecutionGraphNodeProperties>> nodeProps;
            std::vector<flatbuffers::Offset<s::LogicNode>> nodes;

            auto pair              = execGraph.getNodes();
            auto& nonConstantNodes = pair.first;
            auto& constantNodes    = pair.second;

            // serialize the node
            auto serializeNode = [&](auto& map) {
                for(auto& keyValue : map)
                {
                    auto& nodeData = keyValue.second;
                    auto& node     = nodeData.m_node;

                    if(nodeData.m_isAutoGenerated)
                    {
                        continue;  // Skip all internal autogenerated nodes.
                    }
                    nodes.emplace_back(m_nodeSerializer.write(builder, *node));
                }
            };

            // serialize the properties
            auto serializeProps = [&](auto& map, const auto& getGroups) {
                for(auto& keyValue : map)
                {
                    auto& nodeData = keyValue.second;
                    auto& node     = nodeData.m_node;

                    // Serialize Properties
                    std::vector<uint64_t> groups = getGroups(nodeData);

                    auto groupsOffset = builder.CreateVector(groups.data(), groups.size());
                    ExecutionGraphNodePropertiesBuilder nodePropsBuilder(builder);
                    nodePropsBuilder.add_nodeId(node->getId());
                    using NodeClassification = typename GraphType::NodeClassification;
                    auto enumClass           = EnumValuesNodeClassification()[static_cast<std::underlying_type_t<NodeClassification>>(nodeData.m_class)];
                    nodePropsBuilder.add_classification(enumClass);
                    nodePropsBuilder.add_groups(groupsOffset);
                    nodeProps.emplace_back(nodePropsBuilder.Finish());
                }
            };

            serializeNode(nonConstantNodes);
            serializeNode(constantNodes);

            auto getGroups = [](auto& nodeData) {
                return std::vector<uint64_t>(nodeData.m_groups.begin(),
                                             nodeData.m_groups.end());
            };
            serializeProps(nonConstantNodes, getGroups);
            serializeProps(constantNodes, [](auto& nodeData) { return std::vector<uint64_t>{}; });

            return std::make_pair(builder.CreateVector(nodes),
                                  builder.CreateVector(nodeProps));
        }

        //! Serialize all links of a graph `graph` and return the offsets.
        auto writeLinks(flatbuffers::FlatBufferBuilder& builder, const GraphType& execGraph) const
        {
            namespace s = serialization;
            using namespace s;

            std::vector<SocketLinkDescription> socketLinks;

            auto& nonConstantNodes = execGraph.getNodes().first;
            // Iterate over all non-constant nodes.
            for(auto& keyValue : nonConstantNodes)
            {
                auto& node = keyValue.second.m_node;

                for(auto& inputSocket : node->getInputs())
                {
                    // Serialize all Write-Links
                    for(auto& writeSocket : inputSocket->getWritingSockets())
                    {
                        socketLinks.emplace_back(SocketLinkDescription{writeSocket->getParent().getId(),
                                                                       writeSocket->getIndex(),
                                                                       node->getId(),
                                                                       inputSocket->getIndex(),
                                                                       true});
                    }
                    // Serialize Get-Link
                    if(inputSocket->hasGetLink())
                    {
                        SocketOutputBaseType* outSocket = inputSocket->followGetLink();
                        socketLinks.emplace_back(SocketLinkDescription{outSocket->getParent().getId(),
                                                                       outSocket->getIndex(),
                                                                       node->getId(),
                                                                       inputSocket->getIndex(),
                                                                       false});
                    }
                }
            }
            return builder.CreateVectorOfStructs(socketLinks);
        }

    private:
        //! Deserialize a graph `graph` into the internal graph.
        void readGraph(GraphType& execGraph,
                       const serialization::ExecutionGraph& graph)
        {
            auto nodes = graph.nodes();
            if(nodes)
            {
                readNodes(execGraph, *nodes);
            }

            auto links = graph.links();
            if(links)
            {
                readLinks(execGraph, *links);
            }
        }

        //! Deserialize all nodes of a graph `graph` into the internal graph.
        template<typename Nodes>
        void readNodes(GraphType& execGraph, Nodes& nodes)
        {
            for(auto node : nodes)
            {
                std::unique_ptr<NodeBaseType> logicNode = m_nodeSerializer.read(*node);
                if(logicNode)
                {
                    EXECGRAPH_LOG_TRACE("Adding node with id: " << node->id() << " type: " << node->type()->str());
                    execGraph.addNode(std::move(logicNode));
                }
                else
                {
                    EXECGRAPH_THROW("Could not load node with id: '{0}'", node->id());
                }
            }
        }

        //! Deserialize all links of a graph `graph` into the internal graph.
        template<typename Links>
        void readLinks(GraphType& execGraph, Links& links)
        {
            for(auto link : links)
            {
                if(link->isWriteLink())
                {
                    execGraph.addWriteLink(link->outNodeId(),
                                           link->outSocketIdx(),
                                           link->inNodeId(),
                                           link->inSocketIdx());
                }
                else
                {
                    execGraph.setGetLink(link->outNodeId(),
                                         link->outSocketIdx(),
                                         link->inNodeId(),
                                         link->inSocketIdx());
                }
            }
        }

    private:
        std::path m_filePath;                   //!< The temporary file path.
        LogicNodeSerializer& m_nodeSerializer;  //!< The node serializer which provides load/store operations for LogicNodes.
    };
}  // namespace executionGraph

#endif