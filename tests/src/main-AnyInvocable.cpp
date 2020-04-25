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

#include <executionGraph/common/AnyInvocable.hpp>
#include <executionGraph/common/MoveOnly.hpp>
#include "TestFunctions.hpp"

using namespace executionGraph;

struct M : MoveOnly
{
    M(int t)
        : i(t) {}

    int operator()() const { return i; }
    int i;
};

EG_TEST(AnyInvocable, Test1)
{
#if !defined(EG_NO_COMPILE_TEST_INDEX)
    {
        // Ctor of a move only functor
        M m(3);
        AnyInvocable<int()> i = std::move(m);
        ASSERT_EQ(i(), 3);
    }
    {
        // Move ctor of a movable functor
        M m(3);
        AnyInvocable<int()> i(std::move(m));
        auto j = std::move(i);
        ASSERT_EQ(j(), 3);
        ASSERT_TRUE(i == nullptr);
        ASSERT_TRUE(j != nullptr);
    }
    {
        // Ctor and move ctor of copyable/movable lambda
        AnyInvocable<int()> i = []() { return 3; };

        auto j = std::move(i);
        ASSERT_EQ(j(), 3);
        ASSERT_TRUE(i == nullptr);
        ASSERT_TRUE(j != nullptr);

        AnyInvocable<int()> k;
        k = std::move(j);
        ASSERT_EQ(k(), 3);
        ASSERT_TRUE(i == nullptr && j == nullptr);
        ASSERT_TRUE(k != nullptr);
    }
    {
        // Ctor and move ctor of copyable/movable lambda
        M m(3);
        AnyInvocable<int()> i = [m = std::move(m)] { return m(); };

        auto j = std::move(i);
        ASSERT_EQ(j(), 3);
        ASSERT_TRUE(i == nullptr);
        ASSERT_TRUE(j != nullptr);

        AnyInvocable<int()> k;
        k = std::move(j);
        ASSERT_EQ(k(), 3);
        ASSERT_TRUE(i == nullptr && j == nullptr);
        ASSERT_TRUE(k != nullptr);
    }
    {
        // Throwing exception
        M m(3);
        AnyInvocable<int()> i = [m = std::move(m)] { EG_THROW("Wups"); return 3; };

        auto j       = std::move(i);
        bool catched = false;
        try
        {
            j();
        }
        catch(std::exception& e)
        {
            catched = true;
        }
        ASSERT_TRUE(catched) << "Exception not catched";

        catched = false;
        try
        {
            i();
        }
        catch(std::bad_function_call& e)
        {
            catched = true;
        }
        ASSERT_TRUE(catched) << "Exception not catched";
    }
#elif EG_NO_COMPILE_TEST_INDEX == 0
    {
        M m(3);
        AnyInvocable<int()> i(std::move(m));
        auto j = i;  // no copy!!
        // CompileErrorRegex: "AnyInvocable.hpp.*(E|e)rror"
    }
#elif EG_NO_COMPILE_TEST_INDEX == 1
    {
        M m(3);
        AnyInvocable<int()> i(std::move(m));
        AnyInvocable<int()> j;  // no copy!!
        j = i;
        // CompileErrorRegex: "AnyInvocable.hpp.*(E|e)rror"
        // CompileErrorRegex: "cannot*bind.*reference.*of type.*"
    }
#elif EG_NO_COMPILE_TEST_INDEX == 2
    {
        M m(3);
        AnyInvocable<int()> i(std::move(m));
        std::function<int()> j = std::move(i);
        // CompileErrorRegex: "use of deleted function"
    }
#endif
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
