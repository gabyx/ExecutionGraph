// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun May 19 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <type_traits>
#include <utility>
#include <meta/meta.hpp>

namespace meta
{
    namespace detail
    {
        template<typename List, typename F, std::size_t... I>
        constexpr auto invokeFunc(F&& f, meta::index_sequence<I...>)
        {
            return f(std::add_pointer<meta::at_c<List, I>>{}...);
        }
    }  // namespace detail

    template<typename List, typename F>
    constexpr auto invokeFunc(F&& f)
    {
        detail::invokeFunc<List>(std::forward<F>(f), meta::make_index_sequence<List::size()>{});
    }

}  // namespace meta
