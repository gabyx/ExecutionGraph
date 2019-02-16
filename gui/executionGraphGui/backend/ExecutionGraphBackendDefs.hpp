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

#pragma once

#include <meta/meta.hpp>
#include <rttr/type>
#include <executionGraph/nodes/LogicCommon.hpp>
#include <executionGraph/serialization/ExecutionGraphSerializer.hpp>
#include <executionGraph/serialization/LogicNodeSerializer.hpp>
#include "executionGraph/common/Identifier.hpp"
#include "executionGraph/graphs/ExecutionTree.hpp"
#include "executionGraph/serialization/NodeTypeDescription.hpp"
#include "executionGraph/serialization/SocketTypeDescription.hpp"
#include "executionGraphGui/backend/nodes/DummyNode.hpp"
#include "executionGraphGui/backend/nodes/DummyNodeSerializer.hpp"

template<typename TConfig>
class ExecutionGraphBackendDefs;

namespace details
{
    template<typename T>
    struct RegisterAllRTTR;

    template<typename... NodeType>
    struct RegisterAllRTTR<meta::list<NodeType...>>
    {
        static void eval(const std::string& rttrPostfix)
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
            auto l = {(typename NodeType::AutoRegisterRTTR(rttrPostfix), 0)...};
#pragma clang diagnostic pop
        }
    };

}  // namespace details

//! Definitions for a graph with default configuration
template<>
class ExecutionGraphBackendDefs<executionGraph::GeneralConfig<>>
{
public:
    // Get the unique id of this graph type.
    static executionGraph::IdNamed getId()
    {
        return executionGraph::IdNamed{"DefaultGraph", std::string("2992ebff-c950-4184-8876-5fe6ac029aa5")};
    }

    //! The configuration traits.
    using Config = executionGraph::GeneralConfig<>;
    using Graph  = executionGraph::ExecutionTree<Config>;

    //! List of all nodes available in this graph.
    using Nodes                             = meta::list<DummyNode<Config>>;
    static constexpr std::size_t nNodeTypes = meta::size<Nodes>::value;

    using NodeTypeDescription   = executionGraph::NodeTypeDescription;
    using SocketTypeDescription = executionGraph::SocketTypeDescription;

    //! Node serializer for this graph.
    using NodeSerializers = meta::list<DummyNodeSerializer<Config>>;
    using NodeSerializer  = executionGraph::LogicNodeSerializer<Config, NodeSerializers>;

    //! Graph serializer.
    using GraphSerializer = executionGraph::ExecutionGraphSerializer<Graph, NodeSerializer>;

    //! The node descriptions for this default configuration.
    static const std::vector<NodeTypeDescription>& getNodeDescriptions()
    {
        //! The static node description for this default configuration
        static const std::vector<NodeTypeDescription> m_nodeTypeDescriptions = initAllNodeTypeDescriptions();
        return m_nodeTypeDescriptions;
    }

    static const std::string& getDescription()
    {
        static std::string d =
            "This is a default graph for dummy purposes."
            "**Yes you can write markdown here 😁**!";
        return d;
    }

private:
    static std::vector<NodeTypeDescription> initAllNodeTypeDescriptions()
    {
        // Register all types
        details::RegisterAllRTTR<Nodes>::eval("DefaultGraph");

        std::vector<NodeTypeDescription> description;

        // All nodes (here fixed number, to not make it compile)
        const std::array<std::string, 1> nodeNames = {"DummyNode"};
        static_assert(nNodeTypes == nodeNames.size(), "You need a name for each type in NodeTypes");

        using WrappedNodes = meta::transform<Nodes, meta::quote<meta::id>>;

        std::size_t idx          = 0;
        auto emplaceNodeTypeDesc = [&](auto idType) {
            using NodeType = typename decltype(idType)::type;

            //@todo For nodes which do not define this, dispatch to empty namnes...
            auto inSNames  = NodeType::getInputNames();
            auto outSNames = NodeType::getOutputNames();

            description.emplace_back(NodeTypeDescription{rttr::type::get<NodeType>().get_name().to_string(),
                                                         nodeNames[idx],
                                                         {inSNames.begin(), inSNames.end()},
                                                         {outSNames.begin(), outSNames.end()}});
            ++idx;
        };

        meta::for_each(WrappedNodes{}, emplaceNodeTypeDesc);

        return description;
    }
};
