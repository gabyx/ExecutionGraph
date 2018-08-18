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

#ifndef executionGraphGUI_backend_ExecutionGraphBackendDefs_hpp
#define executionGraphGUI_backend_ExecutionGraphBackendDefs_hpp

#include <meta/meta.hpp>
#include <rttr/type>
#include <executionGraph/nodes/LogicCommon.hpp>
#include <executionGraph/serialization/LogicNodeSerializer.hpp>
#include "executionGraph/serialization/NodeTypeDescription.hpp"
#include "executionGraph/serialization/SocketTypeDescription.hpp"
#include "executionGraphGUI/backend/nodes/DummyNode.hpp"
#include "executionGraphGUI/backend/nodes/DummyNodeSerializer.hpp"

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
    //! The configuration traits.
    using Config = executionGraph::GeneralConfig<>;

    //! List of all nodes available in this graph.
    using Nodes                         = meta::list<DummyNode<Config>>;
    static const std::size_t nNodeTypes = meta::size<Nodes>::value;

    using NodeTypeDescription   = executionGraph::NodeTypeDescription;
    using SocketTypeDescription = executionGraph::SocketTypeDescription;

    //! Node serializer for this graph.
    using NodeSerializers = meta::list<DummyNodeSerializer<Config>>;
    using NodeSerializer  = executionGraph::LogicNodeSerializer<Config, NodeSerializers>;

    //! The node descriptions for this default configuration.
    static const std::vector<NodeTypeDescription>& getNodeDescriptions()
    {
        //! The static node description for this default configuration
        static const std::vector<NodeTypeDescription> m_nodeTypeDescriptions = initAllNodeTypeDescriptions();
        return m_nodeTypeDescriptions;
    }

private:
    static std::vector<NodeTypeDescription> initAllNodeTypeDescriptions()
    {
        // Register all types
        details::RegisterAllRTTR<Nodes>::eval("DefaultGraph");

        // All nodes (here fixed number, to not make it compile)
        const std::array<std::string, 1> nodeNames = {"DummyNode"};
        static_assert(nNodeTypes == nodeNames.size(), "You need a name for each type in NodeTypes");

        // All rtti names
        const auto nodeRTTIs = executionGraph::details::MakeRTTIs<Nodes>::eval<std::string>();

        std::vector<NodeTypeDescription> description;
        for(std::size_t idx = 0; idx < nodeNames.size(); ++idx)
        {
            description.emplace_back(NodeTypeDescription{nodeNames[idx], nodeRTTIs[idx]});
        }
        return description;
    }
};

#endif