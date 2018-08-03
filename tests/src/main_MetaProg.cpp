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

#include <meta/meta.hpp>
#include <type_traits>
#include <utility>
#include "TestFunctions.hpp"

struct A
{
    using Writer = int;
};

struct B
{
    using Writer = double;
};

struct C
{
    using NoWriter = double;
};

template<typename T>
using filterWriter = std::is_same<typename T::Writer, int>;

template<typename T, typename = int>
struct hasAlias : std::false_type
{};

template<typename T>
struct hasAlias<T, decltype((void)typename T::Writer{}, 0)> : std::true_type
{};

template<typename T>
using filterWriter = std::is_same<typename T::Writer, int>;

MY_TEST(MetaProgramming, Test1)
{
    {
        using List    = meta::list<A, B>;
        using NewList = meta::filter<List, meta::quote<filterWriter>>;
        static_assert(std::is_same<NewList, meta::list<A>>{});
    }

    {
        using List = meta::list<A, C, B>;
        static_assert(hasAlias<A>{}, "Has Writer");
        static_assert(!hasAlias<C>{}, "Has Writer");

        using NewList = meta::filter<List, meta::quote<hasAlias>>;
        static_assert(std::is_same<NewList, meta::list<A, B>>{});
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
