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

#include "executionGraph/common/MetaIndices.hpp"

namespace executionGraph
{
    namespace tupleUtil
    {
        namespace details
        {
            template<typename Tuple,
                     typename F,
                     std::size_t... Index>
            constexpr void forEach(Tuple&& tuple,
                                   F&& f,
                                   std::index_sequence<Index...>)
            {
                using swallow = int[];
                (void)swallow{0,
                              (f(std::get<Index>(std::forward<Tuple>(tuple))), 0)...};
            }

            template<typename Tuple,
                     typename F,
                     std::size_t... Index>
            constexpr void forEachIdx(Tuple&& tuple,
                                      F&& f,
                                      std::index_sequence<Index...>)
            {
                using swallow = int[];
                (void)swallow{0,
                              (f(std::get<Index>(std::forward<Tuple>(tuple)), Index), 0)...};
            }

            template<typename Tuple,
                     typename F,
                     std::size_t... Index>
            constexpr auto invoke(Tuple&& tuple,
                                  F&& f,
                                  std::index_sequence<Index...>)
            {
                return f(std::get<Index>(std::forward<Tuple>(tuple))...);
            }

            template<std::size_t I,
                     bool doForward = false,
                     typename... Tuple>
            constexpr auto zip_at(Tuple&&... tuple)
            {
                if constexpr(doForward)
                {
                    return std::forward_as_tuple(std::get<I>(std::forward<Tuple>(tuple))...);
                }
                else
                {
                    return std::make_tuple(std::get<I>(std::forward<Tuple>(tuple))...);
                }
            }

            template<bool doForward = false,
                     typename... Tuple,
                     std::size_t... Index>
            constexpr auto zip(std::index_sequence<Index...>,
                               Tuple&&... tuple)
            {
                return std::make_tuple(zip_at<Index, doForward>(std::forward<Tuple>(tuple)...)...);
            }

        }  // namespace details

        //! Execute a functor `f` for each tuple element.
        template<typename Tuple,
                 typename F>
        constexpr void forEach(Tuple&& tuple, F&& f)
        {
            constexpr std::size_t N = std::tuple_size_v<std::remove_cvref_t<Tuple>>;
            details::forEach(std::forward<Tuple>(tuple),
                             std::forward<F>(f),
                             std::make_index_sequence<N>{});
        }

        //! Execute functor `f` for each tuple element with index.
        template<typename Tuple,
                 typename F>
        constexpr void forEachIdx(Tuple&& tuple, F&& f)
        {
            constexpr std::size_t N = std::tuple_size_v<std::remove_cvref_t<Tuple>>;
            details::forEachIdx(std::forward<Tuple>(tuple),
                                std::forward<F>(f),
                                std::make_index_sequence<N>{});
        }

        //! Invoke a functor `f` with all tuple elements injected into.
        //! Optional shuffling with a index list `incices`.
        template<typename Tuple,
                 typename F,
                 typename Index = std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>>
        constexpr auto invoke(Tuple&& tuple,
                              F&& f,
                              Index indices = {})
        {
            return details::invoke(std::forward<Tuple>(tuple),
                                   std::forward<F>(f),
                                   indices);
        }

        // Zip multiple tuples together.
        template<bool doForward = false,
                 typename TupleA,
                 typename... Tuple>
        constexpr auto zip(TupleA&& tuple, Tuple&&... args)
        {
            using namespace meta;

            constexpr auto N = std::tuple_size_v<std::remove_cvref_t<TupleA>>;
            static_assert((... && (N == std::tuple_size_v<std::remove_cvref_t<Tuple>>)),
                          "Not all tuples have the same size");

            return details::zip<doForward>(std::make_index_sequence<N>{},
                                           std::forward<TupleA>(tuple),
                                           std::forward<Tuple>(args)...);
        }

        //! Zip multiple tuples together using
        //! `std::forward_as_tuple`
        template<typename... Tuple>
        constexpr auto zipForward(Tuple&&... tuple)
        {
            return zip<true>(std::forward<Tuple>(tuple)...);
        }

    }  // namespace tupleUtil
}  // namespace executionGraph