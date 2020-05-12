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
#include <executionGraph/common/StaticAssert.hpp>
#include <executionGraph/common/TupleUtil.hpp>
#include <executionGraph/nodes/LogicSocketData.hpp>
#include "TestFunctions.hpp"
using namespace executionGraph;

EG_TEST(SocketData, References)
{
    LogicSocketData<int> n{1};
    LogicSocketData<int> m{2, 6};
    LogicSocketDataRef<int> r{n, 3};
    *r.dataHandle() = 3;
    ASSERT_EQ(*n.dataHandleConst(), 3);
    ASSERT_EQ(*m.dataHandleConst(), 6);
    r.setReference(m);
    *r.dataHandle() = 4;
    ASSERT_EQ(*n.dataHandleConst(), 3);
    ASSERT_EQ(*m.dataHandleConst(), 4);
}

template<typename Data>
void runTest(const Data& init, const Data& write)
{
    LogicSocketData<Data> n{1, init};
    EG_STATIC_ASSERT(std::is_same_v<decltype(*n.dataHandle()), Data&>, "Wrong type");
    EG_STATIC_ASSERT(std::is_same_v<decltype(*n.dataHandleConst()), const Data&>, "Wrong type");
    const auto& cn = n;
    auto n2        = std::move(n);

    auto handle1 = n2.dataHandle();
    ASSERT_EQ(handle1.value(), init);
    *handle1     = write;
    auto handle2 = std::move(handle1);
    ASSERT_EQ(handle2.value(), write);
    ASSERT_TRUE(handle1 == nullptr);
    ASSERT_TRUE(handle2 != nullptr);

    // Cast to const handle
    auto handle2Const = handle2.moveToConst();
    EG_STATIC_ASSERT(std::is_same_v<decltype(*handle2Const), const Data&>, "Wrong type");
    ASSERT_TRUE(handle2 == nullptr);
    ASSERT_TRUE(handle2Const != nullptr);

    // Cast to const handle
    auto handle3Const = staticHandleCast<const Data>(n.dataHandle());
    EG_STATIC_ASSERT(std::is_same_v<decltype(*handle3Const), const Data&>, "Wrong type");
}

template<typename Data, typename CheckerA, typename CheckerB>
void runTestMoveOnly(Data init,
                     Data write,
                     CheckerA&& checkInit,
                     CheckerB&& checkWrite)
{
    LogicSocketData<Data> n{1, std::move(init)};

    auto handle = n.dataHandle();
    ASSERT_TRUE(checkInit(*handle));
    *handle = std::move(write);
    ASSERT_TRUE(checkWrite(*handle));

    // Cast to const handle
    auto handleConst = handle.moveToConst();
    EG_STATIC_ASSERT(std::is_same_v<decltype(*handleConst), const Data&>, "Wrong type");
    ASSERT_TRUE(handle == nullptr);
    ASSERT_TRUE(handleConst != nullptr);

    // Cast to const handle
    {
        auto handleConst = staticHandleCast<const Data>(n.dataHandle());
        EG_STATIC_ASSERT(std::is_same_v<decltype(*handleConst), const Data&>, "Wrong type");
    }
}

EG_TEST(SocketData, HandlesBasic)
{
    runTest(100, 400);
}

EG_TEST(SocketData, HandlesClass)
{
    struct A
    {
        A(int i)
            : a(i) {}

        bool operator==(const A& o) const { return a == o.a; }

        int a;
    };

    runTest(A{100}, A{400});
}

EG_TEST(SocketData, HandlesWithPointers)
{
    LogicSocketData<int*> n{1, nullptr};
    EG_STATIC_ASSERT(std::is_same_v<decltype(*n.dataHandle()), int*&>, "Wrong type");
    EG_STATIC_ASSERT(std::is_same_v<decltype(*n.dataHandleConst()), int* const&>, "Wrong type");

    int a, b;
    runTest(&a, &b);
}

EG_TEST(SocketData, HandlesWithMoveOnly)
{
    struct A
    {
        A(int i)
            : a(i) {}

        A(const A&) = delete;
        A& operator=(const A&) = delete;

        A(A&&) = default;
        A& operator=(A&&) = default;

        bool operator==(const A& o) const { return a == o.a; }

        int a;
    };

    runTestMoveOnly(
        A{100},
        A{400},
        [](const A& v) { return v.a == 100; },
        [](const A& v) { return v.a == 400; });
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
