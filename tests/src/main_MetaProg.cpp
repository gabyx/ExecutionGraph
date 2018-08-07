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
#include <meta/meta.hpp>
#include <string>
#include <type_traits>
#include <utility>
#include "TestFunctions.hpp"

struct Functor
{
    template<typename T>
    void invoke()
    {
        if constexpr(std::is_same<T, char>{})
        {
            std::cout << "char" << std::endl;
        }
        else if constexpr(std::is_same<T, double>{})
        {
            std::cout << "double" << std::endl;
        }
        else
        {
            std::cout << "default" << std::endl;
        }
    }
};

namespace meta
{
    namespace details
    {
        template<typename Func, typename List>
        struct visitTraits;

        template<typename Func, typename... Args>
        struct visitTraits<Func, meta::list<Args...>>
        {
            using MemberPtr = void (Func::*)(void);

            static constexpr std::array<MemberPtr, sizeof...(Args)> makeMap()
            {
                return {&Func::template invoke<Args>...};  // Build member function pointers.
            }
        };
    }  // namespace details

    template<typename List, typename Func>
    void visit(Func&& f, std::size_t index)
    {
        constexpr auto map = details::visitTraits<std::decay_t<Func>, List>::makeMap();
        if(index < map.size())
        {
            (f.*map[index])();
        }
        else
        {
            throw std::out_of_range("Wrong index: " + std::to_string(index));
        }
    }
}  // namespace meta

MY_TEST(MetaProgramming, Test1)
{
    using List = meta::list<int, double, char>;
    int index  = 2;
    Functor func;
    meta::visit<List>(func, index);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
