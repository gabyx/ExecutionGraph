// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Aug 08 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <meta/meta.hpp>
#include "executionGraph/common/Exception.hpp"

namespace meta
{
    namespace details
    {
        template<typename Func, typename Type>
        struct Wrapper
        {
            static_assert(!std::is_same<Type, void>{}, "void type will not work!");
            static auto invoke(Func f)
            {
                return f(Type{});
            }
        };

        template<typename List, typename Func>
        struct visit;

        template<typename Func, typename... Types>
        struct visit<meta::list<Types...>, Func>
        {
            using L = meta::list<Types...>;
            static_assert(meta::size<L>::value != 0, "Cannot visit empty list!");

            static constexpr auto makeMap()
            {
                using FuncPtr = decltype(&Wrapper<Func, meta::front<L>>::invoke);
                using Array   = std::array<FuncPtr, sizeof...(Types)>;
                // Build member function pointers.
                return Array{&Wrapper<Func, Types>::invoke...};
            }
        };

    }  // namespace details

    //! Apply the function `Func` with the type
    //! in the `List` at position `index`.
    template<typename List, typename Func>
    auto visit(std::size_t index, Func&& f)
    {
        using F            = decltype(std::forward<Func>(f));  // Define either an lvalue, or rvalue;
        constexpr auto map = details::visit<List, F>::makeMap();
        EXECGRAPH_THROW_IF(index >= map.size(), "Index {0} >= {1} : out of range!", index, map.size());
        return map[index](std::forward<Func>(f));
    }

}  // namespace meta
