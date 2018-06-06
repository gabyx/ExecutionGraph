// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Jun 06 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#include <executionGraph/nodes/LogicNode.hpp>
#include <executionGraph/nodes/LogicSocket.hpp>
#include <meta/meta.hpp>
#include "DummyNode.hpp"
#include "TestFunctions.hpp"

using namespace executionGraph;

using Config = GeneralConfig<>;

MY_TEST(Node_Test, Int_Int)
{
    // Integer node connection (wrong connection)
    DummyNode<Config> node1(1);
    DummyNode<Config> node2(2);

    node1.addWriteLink(0, node2, 1);  // Correct connection!

    try
    {
        node1.addWriteLink(0, node2, 2);  // Wrong connection!
    }
    catch(NodeConnectionException& e)
    {
        EXECGRAPH_LOG_TRACE("Correct Exception: " << e.what());
        return;
    }

    throw std::runtime_error("Exception not catched!!!");
}

MY_TEST(Node_Test, Int_Int2)
{
    // Integer node connection (wrong connection)
    DummyNode<Config> node1(1);
    DummyNode<Config> node2(2);

    node1.addWriteLink(0, node2, 0);
    node1.addWriteLink(0, node2, 1);

    ASSERT_EQ(node1.getConnectedInputCount(), 0) << "Connected input count wrong";
    ASSERT_EQ(node1.getConnectedOutputCount(), 1) << "Connected input count wrong";
    ASSERT_EQ(node2.getConnectedInputCount(), 2) << "Connected input count wrong";
    ASSERT_EQ(node2.getConnectedOutputCount(), 0) << "Connected input count wrong";
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
