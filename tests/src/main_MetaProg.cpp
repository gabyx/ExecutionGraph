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

#include <array>
#include <exception>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include "TestFunctions.hpp"

#include <executionGraph/common/MetaVisit.hpp>

MY_TEST(MetaProgramming, Test1)
{
    using List = meta::list<int, double, short>;
    int r      = meta::visit<List>(2,
                              [&](auto type) {
                                  using T = decltype(type);
                                  if constexpr(std::is_same<T, short>{})
                                  {
                                      return 2;
                                  }
                                  return -1;
                              });
    ASSERT_EQ(r, 2) << "Meta Visit failed!";
}

MY_TEST(MetaProgramming, Test2)
{
    using List = meta::list<int, double, short>;
    auto f     = [&](auto type) {
        using T = decltype(type);
        if constexpr(std::is_same<T, short>{})
        {
            return 2;
        }
        return -1;
    };

    ASSERT_EQ(meta::visit<List>(2, f), 2) << "Meta Visit failed!";
}
MY_TEST(MetaProgramming, Overloaded)
{
    using List = meta::list<int, double, short>;
    auto f     = overloaded{
        [](int type) { return 0; },
        [](double type) { return 1; },
        [](short type) { return 2; }};

    ASSERT_EQ(meta::visit<List>(2, f), 2) << "Meta Visit failed!";
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
