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

#include <meta/meta.hpp>
#include <executionGraph/graphs/ExecutionTree.hpp>
#include <executionGraph/nodes/LogicNode.hpp>
#include <executionGraph/nodes/LogicSocket.hpp>
#include "DummyNode.hpp"
#include "GraphGenerator.hpp"
#include "TestFunctions.hpp"

using namespace executionGraph;

using Config = GeneralConfig<>;

MY_TEST(ExecutionTree_Test, Int_Int)
{
    using IntNode = DummyNode<Config>;
    auto node1a   = std::make_unique<IntNode>(0, "1a");
    auto node1b   = std::make_unique<IntNode>(1, "1b");

    auto node2a = std::make_unique<IntNode>(2, "2a");
    auto node2b = std::make_unique<IntNode>(3, "2b");

    auto node3a = std::make_unique<IntNode>(4, "3a");
    auto node3b = std::make_unique<IntNode>(5, "3b");

    auto node4a     = std::make_unique<IntNode>(6, "4a");
    auto resultNode = node4a.get();

    try
    {
        node1a->getISocket<double>(0);
        EXECGRAPH_THROW("Should throw exception here!");
    }
    catch(BadSocketCastException& e)
    {
        // all correct
    }
    catch(...)
    {
        EXECGRAPH_THROW("Wrong Exception thrown!");
    }

    // Link
    node4a->setGetLink(*node3a, 0, 0);
    node4a->setGetLink(*node3b, 0, 1);

    node3a->setGetLink(*node1a, 0, 0);
    node3a->setGetLink(*node1b, 0, 1);
    //    node1a->addWriteLink(0,*node3a,0);
    //    node1b->addWriteLink(0,*node3a,1);

    node3b->setGetLink(*node2a, 0, 0);
    node3b->setGetLink(*node2b, 0, 1);
    //node1a->setGetLink(*node3a, 0, 0);  // cycle

    ExecutionTree<Config> execTree;
    execTree.getDefaultOuputPool().setDefaultValue<int>(2);
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

    EXECGRAPH_LOG_TRACE("\n" + execTree.getExecutionOrderInfo());

    execTree.runExecute(0);

    EXECGRAPH_LOG_TRACE("Result : " << resultNode->getOutVal<DummyNode<Config>::Result1>());

    ASSERT_EQ(resultNode->getOutVal<DummyNode<Config>::Result1>(), 16) << "wrong result";

    execTree.removeNode(5);
    execTree.setup();
    execTree.runExecute(0);
    execTree.removeNode(4);
    execTree.setup();
    execTree.runExecute(0);
    ASSERT_EQ(resultNode->getOutVal<DummyNode<Config>::Result1>(), 4) << "wrong result";
}

MY_TEST(ExecutionTree_Test, IntBig)
{
    using IntNode = DummyNode<Config>;
    int nNodes    = 500;

    for(int seed = 0; seed < 10; ++seed)
    {
        {
            auto execTree = createRandomTree<ExecutionTree<Config>, IntNode>(nNodes, seed, false, false);
            execTree->setup(true);
        }
        {
            auto execTree = createRandomTree<ExecutionTree<Config>, IntNode>(nNodes, seed, true, true);
            try
            {
                execTree->setup(true);
            }
            catch(ExecutionGraphCycleException& e)
            {
                // Everything fine
                return;
            }
            catch(...)
            {
                EXECGRAPH_THROW("Wrong Exception thrown!");
            }
            EXECGRAPH_THROW("Added a Cycle but no exception has been thrown!");
        }
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
