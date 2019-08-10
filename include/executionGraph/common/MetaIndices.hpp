// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Mon May 20 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <meta/meta.hpp>

namespace meta
{
    namespace detail
    {
        template<typename L>
        struct to_index_sequence;

        template<typename T, std::size_t... I>
        struct to_index_sequence<list<std::integral_constant<T, I>...>>
        {
            using type = std::index_sequence<I...>;
        };
    }  // namespace detail

    template<typename List>
    using to_index_sequence = _t<detail::to_index_sequence<List>>;
}  // namespace meta

namespace executionGraph
{
    namespace arrayUtil
    {
        //! Convert an `integer_sequence` into an `std::array`
        template<typename T, T... I>
        constexpr auto toArray(std::integer_sequence<T, I...> sequence)
        {
            return std::array<T, sizeof...(I)>{I...};
        }
    }  // namespace arrayUtil
}  // namespace executionGraph