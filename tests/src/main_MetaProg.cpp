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
#include <executionGraph/common/TupleUtil.hpp>

using namespace executionGraph;

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

MY_TEST(MetaProgramming, TupleZip)
{
    std::tuple<int, double, std::string> a = {1, 2.0, "Jeah"};
    std::tuple<double, std::string, int> b = {2.0, "Jeah", 1};

    // Forward can be used in this case.
    auto t = tupleUtil::zip(a, b, std::forward_as_tuple(std::string("Jeah"), 1, 2.0));

    static_assert(std::is_same_v<decltype(t),
                                 std::tuple<decltype(a),
                                            decltype(b),
                                            std::tuple<std::string, int, double>>>,
                  "Type not right!");

    tupleUtil::forEach(t, [](auto&& e) {
        tupleUtil::forEach(
            e,
            overloaded{[](int a) {
                           ASSERT_EQ(a, 1) << "int is not 1";
                       },
                       [](double a) {
                           ASSERT_EQ(a, 2.0) << "double is not 2.0";
                       },
                       [](std::string& a) {
                           ASSERT_EQ(a, "Jeah") << "std::string is not 'Jeah'";
                       }});
    });
}

MY_TEST(MetaProgramming, TupleZipForward)
{
    std::tuple<int, double, std::string> a = {1, 2.0, "Jeah"};
    std::tuple<double, std::string, int> b = {2.0, "Jeah", 1};
    std::tuple<std::string, int, double> c = {"Jeah", 1, 2.0};

    auto t = tupleUtil::zipForward(a, b, std::move(c));
    static_assert(std::is_same_v<decltype(t),
                                 std::tuple<std::tuple<int&, double&, std::string&&>,
                                            std::tuple<double&, std::string&, int&&>,
                                            std::tuple<std::string&, int&, double&&>>>,
                  "Type not right!");

    tupleUtil::forEach(t, [](auto&& e) {
        tupleUtil::forEach(
            e,
            overloaded{[](int a) {
                           ASSERT_EQ(a, 1) << "int is not 1";
                       },
                       [](double a) {
                           ASSERT_EQ(a, 2.0) << "double is not 2.0";
                       },
                       [](std::string& a) {
                           ASSERT_EQ(a, "Jeah") << "std::string is not 'Jeah'";
                       }});
    });
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
