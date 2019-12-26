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
#include <rttr/registration>
#include <executionGraph/common/TupleUtil.hpp>
#include <executionGraph/nodes/LogicNodeData.hpp>
#include "TestFunctions.hpp"
using namespace executionGraph;

MY_TEST(NodeData, Constructor)
{
    LogicNodeData<int> n{1};
    LogicNodeDataRef<int> r{2};

    r.setReference(n);
    *r.data() = 3;
}

MY_TEST(NodeData, HandlesBasic)
{
    LogicNodeData<int> n{1, 100};
    static_assert(std::is_same_v<decltype(*n.data()), int&>, "Wrong type");
    static_assert(std::is_same_v<decltype(*n.cdata()), const int&>, "Wrong type");
    const auto& cn = n;
    static_assert(std::is_same_v<decltype(*cn.data()), const int&>, "Wrong type");
    static_assert(std::is_same_v<decltype(*cn.cdata()), const int&>, "Wrong type");
    auto n2    = std::move(n);
    *n2.data() = 4;
    ASSERT_EQ(*n2.data(), 4);
}

MY_TEST(NodeData, HandlesClass)
{
    struct A
    {
        int a;
    };
    LogicNodeData<A> n{1, 100};
    static_assert(std::is_same_v<decltype(*n.data()), A&>, "Wrong type");
    static_assert(std::is_same_v<decltype(*n.cdata()), const A&>, "Wrong type");
    const auto& cn = n;
    static_assert(std::is_same_v<decltype(*cn.data()), const A&>, "Wrong type");
    static_assert(std::is_same_v<decltype(*cn.cdata()), const A&>, "Wrong type");
    auto n2      = std::move(n);
    n2.data()->a = 4;
    ASSERT_EQ(n2.data()->a, 4);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
