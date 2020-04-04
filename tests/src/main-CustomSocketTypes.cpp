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
#include "TestFunctions.hpp"

#include <memory>
#include <meta/meta.hpp>
#include "executionGraph/graphs/ExecutionTree.hpp"
#include "executionGraph/nodes/LogicNode.hpp"
#include "executionGraph/nodes/LogicSocket.hpp"

using namespace executionGraph;

// A stupid class for a custom SocketTypes
struct A
{
    static const int N = 1 << 4;
    int memory[N];
    A()
    {
        std::fill_n(memory, N, 2);
    }

    A& operator+=(const A& b)
    {
        for(int i = 0; i < N; ++i)
        {
            memory[i] += b.memory[i];
        }
        return *this;
    }
};

// Here we define a Execution Graph with custom Socket types
// with a shared_ptr<A>
using SocketTypes = meta::list<double, std::shared_ptr<A>>;
using Config      = GeneralConfig<SocketTypes>;

template<typename TConfig>
class CustomDummyNode : public TConfig::NodeBaseType
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
    EG_DEFINE_SOCKET_TRAITS(Ins, Outs)

    using InSockets = InSocketDeclList<InSocketDecl<Value1, std::shared_ptr<A>>,
                                       InSocketDecl<Value2, std::shared_ptr<A>>>;

    using OutSockets = OutSocketDeclList<OutSocketDecl<Result1, std::shared_ptr<A>>>;

    EG_DEFINE_LOGIC_NODE_VALUE_GETTERS(Ins, InSockets, Outs, OutSockets)

    template<typename... Args>
    CustomDummyNode(Args&&... args)
        : Base(std::forward<Args>(args)...)
    {
        // Add all sockets
        this->template addSockets<InSockets>();
        this->template addSockets<OutSockets>(std::make_tuple(std::make_shared<A>()));
    }

    void reset() override {}

    void compute() override
    {
        // add the the two inputs (SocketType = shared_ptr<A>)
        // to the output.
        auto sp1   = getInVal<Value1>();
        auto sp2   = getInVal<Value2>();
        auto spOut = getOutVal<Result1>();
        *spOut += *sp1;
        *spOut += *sp2;
    }
};

EG_TEST(ExecutionTree_Test, Int_Int)
{
    // Integer node connection (wrong connection)
    auto node1a = std::make_unique<CustomDummyNode<Config>>(0u);
    auto node1b = std::make_unique<CustomDummyNode<Config>>(1u);

    auto node2a = std::make_unique<CustomDummyNode<Config>>(2u);
    auto node2b = std::make_unique<CustomDummyNode<Config>>(3u);

    auto node3a = std::make_unique<CustomDummyNode<Config>>(4u);
    auto node3b = std::make_unique<CustomDummyNode<Config>>(5u);

    auto node4a     = std::make_unique<CustomDummyNode<Config>>(6u);
    auto resultNode = node4a.get();

    try
    {
        node1a->getISocket<double>(0);
        EG_THROW("Should throw exception here!");
    }
    catch(BadSocketCastException& e)
    {
        // all correct
    }
    catch(...)
    {
        EG_THROW("Wrong Exception thrown!");
    }
    // Link
    node4a->setGetLink(*node3a, 0, 0);
    node4a->setGetLink(*node3b, 0, 1);

    node3a->setGetLink(*node2a, 0, 0);
    node3a->setGetLink(*node2b, 0, 1);

    node3b->setGetLink(*node1a, 0, 0);
    node3b->setGetLink(*node1b, 0, 1);

    ExecutionTree<Config> execTree;
    execTree.getDefaultOuputPool().setDefaultValue<std::shared_ptr<A>>(std::make_shared<A>());
    execTree.addNode(std::move(node1a));
    execTree.addNode(std::move(node1b));
    execTree.addNode(std::move(node2a));
    execTree.addNode(std::move(node2b));
    execTree.addNode(std::move(node3a));
    execTree.addNode(std::move(node3b));
    execTree.addNode(std::move(node4a));

    execTree.setNodeClass(0, ExecutionTree<Config>::NodeClassification::InputNode);
    execTree.setNodeClass(1, ExecutionTree<Config>::NodeClassification::InputNode);
    execTree.setNodeClass(2, ExecutionTree<Config>::NodeClassification::InputNode);
    execTree.setNodeClass(3, ExecutionTree<Config>::NodeClassification::InputNode);
    execTree.setNodeClass(6, ExecutionTree<Config>::NodeClassification::OutputNode);

    execTree.setup();

    EG_LOG_TRACE("{0}", execTree.getExecutionOrderInfo());

    execTree.runExecute(0);

    EG_LOG_TRACE("Result : '{0}'", resultNode->getOutVal<CustomDummyNode<Config>::Result1>()->memory[1]);
    EG_THROW_IF(resultNode->getOutVal<CustomDummyNode<Config>::Result1>()->memory[1] != 30, "wrong result");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
