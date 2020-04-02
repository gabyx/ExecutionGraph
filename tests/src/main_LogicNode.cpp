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
#include <executionGraph/common/Exception.hpp>
#include <executionGraph/common/TupleUtil.hpp>
#include <executionGraph/nodes/LogicSocket.hpp>
#include <executionGraph/nodes/LogicSocketData.hpp>
#include "DummyNode.hpp"
#include "TestFunctions.hpp"

using namespace executionGraph;

MY_TEST(Node_Test, Int_Int)
{
    // Integer node connection (wrong connection)
    LogicSocketData<int> i0(0, 0);
    LogicSocketData<float> i1(1, 0.f);
    LogicSocketData<double> i2(2, 0.0);

    LogicSocketData<int> o0(0, 2);
    LogicSocketData<float> o1(1, 2.0f);
    LogicSocketData<double> o2(2, 2.0);

    DummyNode node1(1);
    node1.socket<node1.in0Decl>().connect(i0);
    node1.socket<node1.in1Decl>().connect(i1);
    node1.socket<node1.in2Decl>().connect(i2);
    node1.socket<node1.out0Decl>().connect(o0);
    node1.socket<node1.out1Decl>().connect(o1);
    node1.socket<node1.out2Decl>().connect(o2);

    node1.compute();
    ASSERT_EQ(o0.dataHandle().get(), 1);
    ASSERT_EQ(o1.dataHandle().get(), 1.f);
    ASSERT_EQ(o2.dataHandle().get(), 1.0);
}

MY_TEST(Node_Test, Wrong_Connections)
{
    // Integer node connection (wrong connection)
    LogicSocketData<int> i(0);
    DummyNode node(0);

    LogicNode* n          = &node;
    bool catchedException = false;
    try
    {
        n->output(1)->connect(i);  // Wrong connection.
    }
    catch(NodeConnectionException&)
    {
        catchedException = true;
    }

    ASSERT_TRUE(catchedException) << "Exception should have been thrown";
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
