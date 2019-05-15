// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed May 15 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <cstddef>
#include <tuple>
#include <utility>

namespace executionGraph
{
    namespace tupleUtil
    {
        namespace details
        {
            template<typename Tuple,
                     typename F,
                     std::size_t... Indices>
            constexpr void forEach(Tuple&& tuple,
                                   F&& f,
                                   std::index_sequence<Indices...>)
            {
                using swallow = int[];
                (void)swallow{0,
                              (f(std::get<Indices>(std::forward<Tuple>(tuple))), 0)...};
            }

            template<typename Tuple,
                     typename F,
                     std::size_t... Indices>
            constexpr void forEachIdx(Tuple&& tuple,
                                      F&& f,
                                      std::index_sequence<Indices...>)
            {
                using swallow = int[];
                (void)swallow{0,
                              (f(std::get<Indices>(std::forward<Tuple>(tuple)), Indices), 0)...};
            }

            template<typename Tuple,
                     typename F,
                     std::size_t... Indices>
            constexpr auto invoke(Tuple&& tuple,
                                  F&& f,
                                  std::index_sequence<Indices...>)
            {
                return f(std::get<Indices>(std::forward<Tuple>(tuple))...);
            }

        }  // namespace details

        //! Execute lambda for each tuple element.
        template<typename Tuple,
                 typename F>
        constexpr void forEach(Tuple&& tuple, F&& f)
        {
            constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<Tuple>>;
            details::forEach(std::forward<Tuple>(tuple),
                             std::forward<F>(f),
                             std::make_index_sequence<N>{});
        }

        //! Execute lambda for each tuple element with index.
        template<typename Tuple,
                 typename F>
        constexpr void forEachIdx(Tuple&& tuple, F&& f)
        {
            constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<Tuple>>;
            details::forEachIdx(std::forward<Tuple>(tuple),
                                std::forward<F>(f),
                                std::make_index_sequence<N>{});
        }

        template<typename Tuple,
                 typename F>
        constexpr auto invoke(Tuple&& tuple, F&& f)
        {
            constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<Tuple>>;
            return details::invoke(std::forward<Tuple>(tuple),
                                   std::forward<F>(f),
                                   std::make_index_sequence<N>{});
        }

    }  // namespace tupleUtil
}  // namespace executionGraph