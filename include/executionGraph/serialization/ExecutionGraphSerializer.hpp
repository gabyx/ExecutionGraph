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
#include "executionGraph/serialization/schemas/ExecutionGraph_generated.h"

namespace executionGraph
{
    namespace serialization
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
        private:
            namespace s = serialization;

        public:
            using Graph = TGraphType;
            EXECGRAPH_TYPEDEF_CONFIG(typename Graph::Config);
            using LogicNodeSerializer = TLogicNodeSerializer;

            ExecutionGraphSerializer(Graph& graph, LogicNodeSerializer& nodeSerializer)
                : m_graph(graph), m_nodeSerializer(nodeSerializer)
            {}
            ~ExecutionGraphSerializer() = default;

        public:
            //! Read an execution graph from a file `filePath`.
            void read(const std::path& filePath) noexcept(false)
            {
                m_filePath = filePath;
                // Memory mapping the file
                FileMapper mapper(m_filePath);
                const uint8_t* buffer = nullptr;
                std::size_t size;
                std::tie(buffer, size) = mapper.getData();
                EXECGRAPH_ASSERT(buffer != nullptr, "FileMapper returned nullptr for file '" << m_filePath << "'");

                // Deserialize
                EXECGRAPH_THROW_EXCEPTION_IF(!s::ExecutionGraphBufferHasIdentifier(buffer),
                                             "File identifier in '" << m_filePath << "' not found!");

                flatbuffers::Verifier verifier(buffer, size, 64, 1000000000);
                EXECGRAPH_THROW_EXCEPTION_IF(!s::VerifyExecutionGraphBuffer(verifier),
                                             "Buffer in '" << m_filePath << "' could not be verified!");

                auto graph = s::GetExecutionGraph(buffer);

                EXECGRAPH_THROW_EXCEPTION_IF(graph == nullptr,
                                             "Deserialization from '" << m_filePath << "' is invalid!");

                readGraph(*graph);
            }

            //! Write an execution graph to the file `filePath`.
            void write(const std::path& filePath, bool bOverwrite = false) noexcept(false);
            {
                flatbuffers::FlatBufferBuilder builder;
                auto graphOffset = writeGraph(builder, m_graph);
                s::FinishExecutionGraphBuffer(builder, graphOffset);

                std::ofstream file;
                EXECGRAPH_THROW_EXCEPTION_IF(!bOverwrite && std::filesystem::exists(filePath),
                                             "File '" << filePath << "' already exists!");

                file.open(filePath.string(), std::ios_base::trunc | std::ios_base::binary | std::ios_base::in);
                file.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
                file.close();
            }

        private:
            flatbuffers::Offset<s::ExecutionGraph> writeGraph(flatbuffers::FlatBufferBuilder& builder, const Graph& graph) const
            {
                flatbuffers::Offset<flatbuffers::Vector<s::LogicNode>> nodesOffset;
                flatbuffers::Offset<flatbuffers::Vector<s::ExecutionGraphNodeProperties>> nodePropertiesOffset;

                std::tie(nodesOffset, nodePropertiesOffset) = writeNodes(builder, graph);
                auto linksOffset                            = writeLinks(builder, graph);

                s::ExecutionGraphBuilder graphBuilder(builder);
                graphBuilder.add_nodes(nodesOffset);
                graphBuilder.add_nodeProperties(nodePropertiesOffset);
                graphBuilder.add_links(linksOffset);
            }

            void writeNodes(flatbuffers::FlatBufferBuilder& builder, const Graph& graph) const
            {
                writeNodes(builder, graph);
            }

        private:
            void readGraph(const s::ExecutionGraph& graph)
            {
                auto nodes = graph.nodes();
                if(nodes)
                {
                    readNodes(*nodes);
                }

                auto links = graph.links();
                if(links)
                {
                    readLinks(*links);
                }
            }

            template<typename Nodes>
            void readNodes(Nodes& nodes)
            {
                for(auto node : nodes)
                {
                    std::unique_ptr<NodeBaseType> logicNode = m_nodeSerializer.read(*node);
                    if(logicNode)
                    {
                        EXECGRAPH_LOG_TRACE("Adding node with id: " << node->id());
                        m_graph.addNode(std::move(logicNode));
                    }
                    else
                    {
                        EXECGRAPH_LOG_TRACE("Could not load node with id: " << node->id());
                    }
                }
            }

            template<typename Links>
            void readLinks(Links& links)
            {
                for(auto link : links)
                {
                }
            }

        private:
            std::path m_filePath;                   //!< The temporary file path.
            Graph& m_graph;                         //!< The graph which is stored or deserialized into.
            LogicNodeSerializer& m_nodeSerializer;  //!< The node serializer which provides load/store operations for LogicNodes.
        };
    }  // namespace serialization

}  // namespace executionGraph

#endif