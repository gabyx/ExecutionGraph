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

#include <meta/meta.hpp>
#include "executionGraph/common/MetaCommon.hpp"
#include "executionGraph/common/MetaIndices.hpp"
#include "executionGraph/common/SfinaeMacros.hpp"

namespace executionGraph
{
    namespace tupleUtil
    {
        namespace details
        {
            template<auto F>
            struct lambdaQuote
            {
                template<typename... T>
                using invoke = decltype(F(T{}...));
            };

            template<typename Tuple,
                     typename F,
                     std::size_t... Index>
            constexpr void forEach(Tuple&& t,
                                   F&& f,
                                   std::index_sequence<Index...>)
            {
                using swallow = int[];
                (void)swallow{0,
                              (f(std::get<Index>(std::forward<Tuple>(t))), 0)...};
            }

            template<typename Tuple,
                     typename F,
                     std::size_t... Index>
            constexpr void forEachIdx(Tuple&& t,
                                      F&& f,
                                      std::index_sequence<Index...>)
            {
                using swallow = int[];
                (void)swallow{0,
                              (f(std::get<Index>(std::forward<Tuple>(t)), Index), 0)...};
            }

            template<std::size_t Index, typename Tuple>
            constexpr decltype(auto) get(Tuple&& t)
            {
                using Type = std::tuple_element_t<Index, naked<Tuple>>;
                if constexpr(std::is_rvalue_reference_v<Type>)
                {
                    return std::move(std::get<Index>(t));
                }
                else
                {
                    return std::get<Index>(t);
                }
            }

            template<typename Tuple,
                     typename F,
                     std::size_t... Index>
            constexpr decltype(auto) invoke(Tuple&& t,
                                            F&& f,
                                            std::index_sequence<Index...>)
            {
                return f(details::get<Index>(std::forward<Tuple>(t))...);
            }

            template<std::size_t I,
                     bool doForward = false,
                     typename... Tuple>
            constexpr auto zip_at(Tuple&&... t)
            {
                if constexpr(doForward)
                {
                    return std::forward_as_tuple(std::get<I>(std::forward<Tuple>(t))...);
                }
                else
                {
                    return std::make_tuple(std::get<I>(std::forward<Tuple>(t))...);
                }
            }

            template<bool doForward = false,
                     typename... Tuple,
                     std::size_t... Index>
            constexpr auto zip(std::index_sequence<Index...>,
                               Tuple&&... t)
            {
                return std::make_tuple(zip_at<Index, doForward>(std::forward<Tuple>(t)...)...);
            }

            template<auto t, auto pred, bool doForward = false>
            constexpr auto sort()
            {
                using namespace meta;
                using namespace details;
                using Tuple = naked<decltype(t)>;

                using Pred = lambdaQuote<
                    [](auto I, auto J) {
                        return bool_<pred(std::get<I>(t), std::get<J>(t))>{};
                    }>;

                using Indices       = as_list<make_index_sequence<std::tuple_size_v<Tuple>>>;
                using SortedIndices = meta::sort<Indices, Pred>;
                auto createTuple    = []<typename... Ts>(list<Ts...>)
                {
                    if constexpr(doForward)
                    {
                        return std::forward_as_tuple(std::get<Ts::value>(t)...);
                    }
                    else
                    {
                        return std::make_tuple(std::get<Ts::value>(t)...);
                    }
                };

                return createTuple(SortedIndices{});
            }

        }  // namespace details

        //! Execute a functor `f` for each tuple element.
        template<typename Tuple,
                 typename F,
                 EG_ENABLE_IF(meta::is<naked<Tuple>, std::tuple>::value)>
        constexpr void forEach(Tuple&& t, F&& f)
        {
            constexpr std::size_t N = std::tuple_size_v<naked<Tuple>>;
            details::forEach(std::forward<Tuple>(t),
                             std::forward<F>(f),
                             std::make_index_sequence<N>{});
        }

        //! Execute functor `f` for each tuple element with index.
        template<typename Tuple,
                 typename F,
                 EG_ENABLE_IF(meta::is<naked<Tuple>, std::tuple>::value)>
        constexpr void forEachIdx(Tuple&& t, F&& f)
        {
            constexpr std::size_t N = std::tuple_size_v<naked<Tuple>>;
            details::forEachIdx(std::forward<Tuple>(t),
                                std::forward<F>(f),
                                std::make_index_sequence<N>{});
        }

        //! Invoke a functor `f` with all tuple elements injected into.
        //! Optional shuffling with a index list `incices`.
        template<typename Tuple,
                 typename F,
                 typename Index = std::make_index_sequence<std::tuple_size_v<naked<Tuple>>>,
                 EG_ENABLE_IF(meta::is<naked<Tuple>, std::tuple>::value)>
        constexpr decltype(auto) invoke(Tuple&& t,
                                        F&& f,
                                        Index indices = {})
        {
            return details::invoke(std::forward<Tuple>(t),
                                   std::forward<F>(f),
                                   indices);
        }

        template<typename Tuple,
                 typename F,
                 typename Index = std::make_index_sequence<std::tuple_size_v<naked<Tuple>>>,
                 EG_ENABLE_IF(meta::is<naked<Tuple>, std::tuple>::value)>
        constexpr decltype(auto) indexed(Tuple&& t,
                                        F&& f,
                                        Index indices = {})
        {
            return f(std::forward<Tuple>(t), indices);
        }

        // Zip multiple tuples together.
        template<bool doForward = false,
                 typename Tuple,
                 typename... Tuples,
                 EG_ENABLE_IF((meta::is<naked<Tuple>, std::tuple>::value && ... && meta::is<naked<Tuples>, std::tuple>::value))>
        constexpr auto zip(Tuple&& t, Tuples&&... tt)
        {
            using namespace meta;

            constexpr auto N = std::tuple_size_v<naked<Tuple>>;
            static_assert((... && (N == std::tuple_size_v<naked<Tuples>>)),
                          "Not all tuples have the same size");

            return details::zip<doForward>(std::make_index_sequence<N>{},
                                           std::forward<Tuple>(t),
                                           std::forward<Tuples>(tt)...);
        }

        //! Zip multiple tuples together using
        //! `std::forward_as_tuple`
        template<typename... Tuple>
        constexpr decltype(auto) zipForward(Tuple&&... t)
        {
            return zip<true>(std::forward<Tuple>(t)...);
        }

        //! Sorting a constexpr `tuple` by a constexpr `pred`.
        template<auto t, auto pred>
        constexpr auto sort()
        {
            return details::sort<t, pred>();
        }

        //! Sorting a constexpr `tuple` by a constexpr `pred` using
        //! `std::forward_as_tuple`
        template<auto t, auto pred>
        constexpr auto sortForward()
        {
            return details::sort<t, pred, true>();
        }

        template<typename Tuple,
                 EG_ENABLE_IF(meta::is<naked<Tuple>, std::tuple>::value)>
        constexpr auto forward(Tuple&& t)
        {
            return invoke(t,
                          [](auto&&... args) { return std::forward_as_tuple(
                                                   std::forward<decltype(args)>(args)...); });
        }

        template<auto& t,
                 EG_ENABLE_IF(meta::is<naked<decltype(t)>, std::tuple>::value)>
        constexpr auto forwardS()
        {
            return invoke(t,
                          [](auto&&... args) { return std::forward_as_tuple(
                                                   std::forward<decltype(args)>(args)...); });
        }
    }  // namespace tupleUtil
}  // namespace executionGraph