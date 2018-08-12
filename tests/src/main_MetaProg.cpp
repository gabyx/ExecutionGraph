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
#include <string>
#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include "TestFunctions.hpp"

#include <executionGraph/common/MetaVisit.hpp>

struct Functor
{
    template<typename T>
    void invoke(int b)
    {
        if constexpr(std::is_same<T, char>{})
        {
            std::cout << "char" << b << std::endl;
        }
        else if constexpr(std::is_same<T, double>{})
        {
            throw std::runtime_error("this should not be thrown!");
        }
        else
        {
            throw std::runtime_error("this should not be thrown!");
        }
    }
};

MY_TEST(MetaProgramming, Test1)
{
    using List = meta::list<int, double, char>;
    Functor func;
    meta::visit<List>(func, 2, 3);

    try
    {
        meta::visit<List>(func, 1, 3);
        throw std::runtime_error("no exception thrown!");
    }
    catch(...)
    {
    }

    try
    {
        meta::visit<List>(func, 3, 3);
        throw std::runtime_error("no exception thrown!");
    }
    catch(...)
    {
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
