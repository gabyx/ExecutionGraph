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
#include "TestFunctions.hpp"
#include <rttr/registration>
using namespace executionGraph;

MY_TEST(NodeData, Constructor)
{
    LogicNodeData<int> n{1};
    LogicNodeDataRef<int> r{2};

    r.setReference(n);
    *r.data() = 3;
}

MY_TEST(NodeData, Handles)
{
    LogicNodeData<int> n{1};

    n.data();
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
