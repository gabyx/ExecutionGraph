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

MY_TEST(MetaProgramming, Invoke)
{
    struct A
    {
        int v;
    };

    A a;
    A& b = a;

    auto t  = std::make_tuple(A{1}, A{2}, A{3});
    auto& r = tupleUtil::invoke(t, [&](auto&&... v) -> decltype(auto) {
        return b;
    });

    static_assert(std::is_same_v<decltype(r), A&>, "Wrong Type");
}


template<typename T>
constexpr bool testTupleInvokeImpl(T&& v)
{
    return v.v <= 3;
}

template<auto& t>
constexpr bool testTupleInvoke()
{
    return tupleUtil::invoke(t, [](auto&&... v) {
        return (... && testTupleInvokeImpl(v));
    });
}

MY_TEST(MetaProgramming, InvokeConstExpr)
{
    struct A
    {
        int v;
    };

    static constexpr auto t = std::make_tuple(A{1}, A{2}, A{3});
    static_assert(testTupleInvoke<t>(), "Wrong Type");
}

MY_TEST(MetaProgramming, InvokeForward)
{
    struct A
    {};
    A a, b, c;
    auto t = std::forward_as_tuple(a, b, std::move(c));

    tupleUtil::invoke(std::move(t), [&](auto&& arg1, auto&& arg2, auto&& arg3) {
        static_assert(std::is_same_v<decltype(arg1), A&> &&
                          std::is_same_v<decltype(arg2), A&> &&
                          std::is_same_v<decltype(arg3), A&&>,
                      "Wrong Types");
        return true;
    });

    tupleUtil::invoke(t, [&](auto&& arg1, auto&& arg2, auto&& arg3) {
        static_assert(std::is_same_v<decltype(arg1), A&> &&
                          std::is_same_v<decltype(arg2), A&> &&
                          std::is_same_v<decltype(arg3), A&&>,
                      "Wrong Types");
        return true;
    });

    std::apply(
        [&](auto&& arg1, auto&& arg2, auto&& arg3) {
            static_assert(std::is_same_v<decltype(arg1), A&> &&
                              std::is_same_v<decltype(arg2), A&> &&
                              std::is_same_v<decltype(arg3), A&>,  // A lvalue-reference here is the behavior of `std::get`
                          "Wrong Types");
            return true;
        },
        t);
}

template<typename T>
struct A
{
    constexpr A(T t)
        : val(t){};
    T val;

private:
    constexpr A() = default;
};

template<auto&... d>
constexpr auto sortWrapper()
{
    constexpr auto p = [](auto&& a, auto&& b) { return a.val < b.val; };
    // `d` needs static storage duration because we take addresses here
    constexpr auto t = std::forward_as_tuple(d...);
    return tupleUtil::sortForward<t, p>();
}

MY_TEST(MetaProgramming, TupleSort)
{
    static constexpr auto t = std::make_tuple(A{10.0}, A{9}, A{8.0f}, A{7.0}, A{6.0}, A{5});
    constexpr auto p        = [](auto&& a, auto&& b) { return a.val < b.val; };

    {
        constexpr auto s          = tupleUtil::sort<t, p>();
        static constexpr auto res = std::make_tuple(A{5}, A{6.0}, A{7.0}, A{8.0f}, A{9}, A{10.0});
        static_assert(std::is_same_v<decltype(s), decltype(res)>, "not correct");
        static_assert(std::get<0>(res).val == std::get<0>(s).val, "not correct");

        constexpr auto sF   = tupleUtil::sortForward<t, p>();
        constexpr auto resF = tupleUtil::toReferences(res);
        static_assert(std::is_same_v<decltype(sF), decltype(resF)>, "not correct");
        static_assert(std::get<0>(resF).val == std::get<0>(sF).val, "not correct");
    }

    {
        static constexpr A a{1};
        static constexpr A b{2};
        constexpr auto t = std::forward_as_tuple(b, a);
        static_assert(&a == &std::get<1>(t), "address should be the same");

        constexpr auto tf = tupleUtil::toReferences(t);
        static_assert(&b == &std::get<0>(tf), "address should be the same");

        constexpr auto s1 = tupleUtil::sortForward<tf, p>();
        static_assert(&b == &std::get<1>(s1), "address should be the same");

        constexpr auto s2 = tupleUtil::sortForward<std::forward_as_tuple(b, a), p>();
        static_assert(&b == &std::get<1>(s2), "address should be the same");

        constexpr auto s3 = tupleUtil::sort<std::forward_as_tuple(b, a), p>();
        static_assert(&b != &std::get<1>(s3), "address should be the same");

        constexpr auto s4 = sortWrapper<b, a>();
        static_assert(&b == &std::get<1>(s4), "address should be the same");
    }
}

MY_TEST(MetaProgramming, ConstexprForward)
{
    // Compile time...
    static constexpr auto t = std::make_tuple(A{10.0}, A{9}, A{8.0f});
    constexpr auto tF       = tupleUtil::toReferences(t);
    static_assert(std::is_same_v<decltype(tF),
                                 const std::tuple<const A<double>&,
                                                  const A<int>&,
                                                  const A<float>&>>,
                  "not correct");
    // Runtime...
    auto t2  = std::make_tuple(A{10.0}, A{9}, A{8.0f});
    auto tF2 = tupleUtil::toReferences(t2);
    static_assert(std::is_same_v<decltype(tF2),
                                 std::tuple<A<double>&,
                                            A<int>&,
                                            A<float>&>>,
                  "not correct");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
