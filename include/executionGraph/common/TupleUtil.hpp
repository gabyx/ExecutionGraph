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

        //! Execute a functor `f` for each tuple element.
        template<typename Tuple,
                 typename F>
        constexpr void forEach(Tuple&& tuple, F&& f)
        {
            constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<Tuple>>;
            details::forEach(std::forward<Tuple>(tuple),
                             std::forward<F>(f),
                             std::make_index_sequence<N>{});
        }

        //! Execute functor `f` for each tuple element with index.
        template<typename Tuple,
                 typename F>
        constexpr void forEachIdx(Tuple&& tuple, F&& f)
        {
            constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<Tuple>>;
            details::forEachIdx(std::forward<Tuple>(tuple),
                                std::forward<F>(f),
                                std::make_index_sequence<N>{});
        }

        //! Invoke a functor `f` with all tuple elements injected into.
        //! Optional shuffling with a index list `Incides`.
        template<typename Indices = void,
                 typename Tuple,
                 typename F>
        constexpr auto invoke(Tuple&& tuple, F&& f)
        {
            using Idx = std::conditional_t<std::is_same_v<Indices, void>,
                                           std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>,
                                           Indices>;
            return details::invoke(std::forward<Tuple>(tuple),
                                   std::forward<F>(f),
                                   Idx{});
        }

    }  // namespace tupleUtil
}  // namespace executionGraph