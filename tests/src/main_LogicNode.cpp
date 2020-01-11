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

//#include <executionGraph/nodes/LogicNode.hpp>
#include <executionGraph/common/TupleUtil.hpp>
#include <executionGraph/nodes/LogicNodeData.hpp>
#include <executionGraph/nodes/LogicSocket.hpp>
#include "DummyNode.hpp"
#include "TestFunctions.hpp"

using namespace executionGraph;

MY_TEST(Node_Test, Int_Int)
{
    // Integer node connection (wrong connection)
    LogicNodeData<float> f(1);
    DummyNode node1(1);
    DummyNode node2(2);
    node1.socket<node1.out2Decl>().connect(f);
    //node2.socket(node2.in2Decl)·connect(f);

    //node1.addWriteLink(0, node2, 1);  // Correct connection!

    // try
    // {
    // node1.addWriteLink(0, node2, 2);  // Wrong connection!
    // }
    // catch(NodeConnectionException& e)
    // {
    // EG_LOG_TRACE("Correct Exception: '{0}'", e.what());
    // return;
    // }

    // throw std::runtime_error("Exception not catched!!!");
}

// MY_TEST(Node_Test, Int_Int2)
// {
//     // Integer node connection (wrong connection)
//     DummyNode node1(1);
//     DummyNode node2(2);

//     node1.addWriteLink(0, node2, 0);
//     node1.addWriteLink(0, node2, 1);

//     ASSERT_EQ(node1.connectedInputCount(), 0) << "Connected input count wrong";
//     ASSERT_EQ(node1.connectedOutputCount(), 1) << "Connected input count wrong";
//     ASSERT_EQ(node2.connectedInputCount(), 2) << "Connected input count wrong";
//     ASSERT_EQ(node2.connectedOutputCount(), 0) << "Connected input count wrong";
// }

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
