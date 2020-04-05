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
#include <executionGraph/common/StaticAssert.hpp>
#include <executionGraph/common/TupleUtil.hpp>
#include <executionGraph/nodes/LogicSocketData.hpp>
#include "TestFunctions.hpp"
using namespace executionGraph;

EG_TEST(SocketData, Constructor)
{
    LogicSocketData<int> n{1};
    LogicSocketDataRef<int> r{2};

    r.setReference(n);
    *r.dataHandle() = 3;
}

EG_TEST(SocketData, HandlesBasic)
{
    LogicSocketData<int> n{1, 100};
    EG_STATIC_ASSERT(std::is_same_v<decltype(*n.dataHandle()), int&>, "Wrong type");
    EG_STATIC_ASSERT(std::is_same_v<decltype(*n.dataHandleConst()), const int&>, "Wrong type");
    const auto& cn = n;
    EG_STATIC_ASSERT(std::is_same_v<decltype(*cn.dataHandle()), const int&>, "Wrong type");
    EG_STATIC_ASSERT(std::is_same_v<decltype(*cn.dataHandleConst()), const int&>, "Wrong type");
    auto n2      = std::move(n);
    auto handle1 = n2.dataHandle();
    *handle1     = 4;
    auto handle2 = std::move(handle1);
    ASSERT_TRUE(handle1 == nullptr);
    ASSERT_TRUE(handle2 != nullptr);
    ASSERT_EQ(*n2.dataHandle(), 4);
}

EG_TEST(SocketData, HandlesClass)
{
    struct A
    {
        int a;
    };
    LogicSocketData<A> n{1, 100};
    EG_STATIC_ASSERT(std::is_same_v<decltype(*n.dataHandle()), A&>, "Wrong type");
    EG_STATIC_ASSERT(std::is_same_v<decltype(*n.dataHandleConst()), const A&>, "Wrong type");
    const auto& cn = n;
    EG_STATIC_ASSERT(std::is_same_v<decltype(*cn.dataHandle()), const A&>, "Wrong type");
    EG_STATIC_ASSERT(std::is_same_v<decltype(*cn.dataHandleConst()), const A&>, "Wrong type");
    auto n2      = std::move(n);
    auto handle1 = n2.dataHandle();
    handle1->a   = 4;
    auto handle2 = std::move(handle1);
    ASSERT_TRUE(handle1 == nullptr);
    ASSERT_TRUE(handle2 != nullptr);
    ASSERT_EQ(n2.dataHandle()->a, 4);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
