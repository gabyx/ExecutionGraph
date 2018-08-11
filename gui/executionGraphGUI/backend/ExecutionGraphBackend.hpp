//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Sun Jan 14 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraphGUI_backend_ExecutionGraphBackend_hpp
#define executionGraphGUI_backend_ExecutionGraphBackend_hpp

#include <array>
#include <executionGraph/common/Identifier.hpp>
#include <executionGraph/graphs/ExecutionTreeInOut.hpp>
#include <executionGraph/serialization/GraphTypeDescription.hpp>
#include <rttr/type>
#include <string>
#include <variant>
#include <vector>
#include "executionGraphGUI/backend/Backend.hpp"
#include "executionGraphGUI/backend/ExecutionGraphBackendDefs.hpp"
#include "executionGraphGUI/common/RequestError.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    The execution graph backend.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class ExecutionGraphBackend final : public Backend
{
    RTTR_ENABLE()

public:
    using DefaultGraph         = executionGraph::ExecutionTreeInOut<executionGraph::GeneralConfig<>>;
    using Id                   = executionGraph::Id;
    using IdNamed              = executionGraph::IdNamed;
    using GraphTypeDescription = executionGraph::GraphTypeDescription;
    using NodeId               = executionGraph::NodeId;

public:
    ExecutionGraphBackend()
        : Backend(IdNamed("ExecutionGraphBackend"))
    {
    }
    ~ExecutionGraphBackend() override = default;

    //! Adding/removing graphs.
    //@{
    void addGraph(const Id& graphId,
                  const Id& graphType);
    void removeGraph(const Id& graphId);
    void removeGraphs();
    //@}

    //! Adding/removing nodes.
    //@{
    template<typename ResponseCreator>
    void addNode(const Id& graphId,
                 const std::string& type,
                 const std::string& nodeName,
                 ResponseCreator&& responseCreator);

    void removeNode(const Id& graphId,
                    NodeId id,
                    const std::function<void()>& responseCreator);
    //@}

    //! Information about graphs.
    //@{
public:
private:
    static const std::array<IdNamed, 1> m_graphTypeDescriptionIds;                     //!< All IDs used for the graph description.
    static const std::unordered_map<Id, GraphTypeDescription> m_graphTypeDescription;  //!< All graph descriptions.

public:
    //! Get all graph descriptions identified by its id.
    const std::unordered_map<Id, GraphTypeDescription>& getGraphTypeDescriptions() const { return m_graphTypeDescription; }
    //@}
private:
    using GraphVariant = std::variant<DefaultGraph>;
    //! Map of normal graphs identified by its id.
    std::unordered_map<Id, GraphVariant> m_graphs;
};

//! Add a node with type `type` to the graph with id `graphId`.
template<typename ResponseCreator>
void ExecutionGraphBackend::addNode(const Id& graphId,
                                    const std::string& type,
                                    const std::string& nodeName,
                                    ResponseCreator&& responseCreator)
{
    auto graphIt = m_graphs.find(graphId);
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(graphIt == m_graphs.end(),
                                      "Graph id: '{0}' does not exist!",
                                      graphId.toString());
    GraphVariant& graphVar = graphIt->second;

    auto addNodeLambda = [&](auto& graph) {
        using GraphType    = decltype(graph);
        using Config       = typename GraphType::Config;
        using NodeBaseType = typename Config::NodeBaseType;

        // Construct the node with the serializer
        typename ExecutionGraphBackendDefs<Config>::NodeSerializer serializer;
        NodeId id          = graph.generateNodeId();
        NodeBaseType* node = nullptr;

        try
        {
            auto n = serializer.read(type, id, nodeName);
            node   = graph->addNode(std::move(n));
        }
        catch(executionGraph::Exception& e)
        {
            EXECGRAPHGUI_THROW_TYPE(BadRequestError,
                                    "Construction of node '{0}' with type: '{1}' for graph id '{2}' failed: '{3}'",
                                    nodeName,
                                    type,
                                    graphId,
                                    e.what());
        }

        EXECGRAPH_ASSERT(node != nullptr, "Node is nullptr!!?");

        // Create the response
        responseCreator(graph, *node);
    };

    std::visit(addNodeLambda, graphVar);
}

#endif
