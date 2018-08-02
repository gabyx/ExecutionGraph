//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 08 2018
//!  @author Gabriel Nützi, <gnuetzi (at) gmail (døt) com>
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================
#include "executionGraph/graphs/ExecutionTreeInOut.hpp"
#include "executionGraph/nodes/LogicNode.hpp"
#include "executionGraph/nodes/LogicSocket.hpp"

using namespace executionGraph;

using Config = GeneralConfig<>;

template<typename TConfig>
class IntegerNode : public TConfig::NodeBaseType
{
public:
    using Config = TConfig;
    using Base   = typename Config::NodeBaseType;

    enum Ins
    {
        Value1,
        Value2
    };
    enum Outs
    {
        Result1,
    };
    EXECGRAPH_DEFINE_SOCKET_TRAITS(Ins, Outs);

    using InSockets = InSocketDeclList<InSocketDecl<Value1, int>,
                                       InSocketDecl<Value2, int>>;

    using OutSockets = OutSocketDeclList<OutSocketDecl<Result1, int>>;

    EXECGRAPH_DEFINE_LOGIC_NODE_GET_TYPENAME()
    EXECGRAPH_DEFINE_LOGIC_NODE_VALUE_GETTERS(Ins, InSockets, Outs, OutSockets)

    template<typename... Args>
    IntegerNode(Args&&... args)
        : Base(std::forward<Args>(args)...)
    {
        // Add all sockets
        this->template addSockets<InSockets>();
        this->template addSockets<OutSockets>(std::make_tuple(0));
    }

    void reset() override {}

    void compute() override
    {
        getOutVal<Result1>() = getInVal<Value1>() + getInVal<Value2>();
    }
};

int main()
{
    // Define the nodes
    using IntNode = IntegerNode<Config>;
    auto node1a   = std::make_unique<IntNode>(0, "1a");
    auto node1b   = std::make_unique<IntNode>(1, "1b");

    auto node2a = std::make_unique<IntNode>(2, "2a");
    auto node2b = std::make_unique<IntNode>(3, "2b");

    auto node3a = std::make_unique<IntNode>(4, "3a");
    auto node3b = std::make_unique<IntNode>(5, "3b");

    auto node4a     = std::make_unique<IntNode>(6, "4a");
    auto resultNode = node4a.get();

    // Link all nodes together
    node4a->setGetLink(*node3a, 0, 0);
    node4a->setGetLink(*node3b, 0, 1);

    node3a->setGetLink(*node1a, 0, 0);
    node3a->setGetLink(*node1b, 0, 1);

    node3b->setGetLink(*node2a, 0, 0);
    node3b->setGetLink(*node2b, 0, 1);
    //node1a->setGetLink(*node1a,0,0); // this would make cycle which is prohibited

    // Add all nodes to the execution tree
    ExecutionTreeInOut<Config> execTree;
    execTree.addNode(std::move(node1a));
    execTree.addNode(std::move(node1b));
    execTree.addNode(std::move(node2a));
    execTree.addNode(std::move(node2b));
    execTree.addNode(std::move(node3a));
    execTree.addNode(std::move(node3b));
    execTree.addNode(std::move(node4a));

    // Set the classification of the nodes
    execTree.setNodeClass(0, ExecutionTreeInOut<Config>::NodeClassification::InputNode);
    execTree.setNodeClass(1, ExecutionTreeInOut<Config>::NodeClassification::InputNode);
    execTree.setNodeClass(2, ExecutionTreeInOut<Config>::NodeClassification::InputNode);
    execTree.setNodeClass(3, ExecutionTreeInOut<Config>::NodeClassification::InputNode);
    execTree.setNodeClass(6, ExecutionTreeInOut<Config>::NodeClassification::OutputNode);

    // Build the execution order
    execTree.setup();
    EXECGRAPH_LOG_INFO(execTree.getExecutionOrderInfo());
    // Execute the default group 0
    execTree.runExecute(0);

    EXECGRAPH_LOG_INFO("Result : " << resultNode->getOutVal<IntegerNode<Config>::Result1>());
}
