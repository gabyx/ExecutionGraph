// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Fri Jan 03 2020
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <algorithm>
#include <functional>

namespace executionGraph
{
    namespace cx
    {
        //! std::is_sorted_until
        //! Taken from https://github.com/gcc-mirror/gcc/blob/d9375e490072d1aae73a93949aa158fcd2a27018/libstdc%2B%2B-v3/include/bits/stl_algo.h#L3193
        template<typename ForwardIterator,
                 typename DefaultComp = std::less<void>,
                 typename Compare     = DefaultComp>
        constexpr ForwardIterator
        is_sorted_until(ForwardIterator first, ForwardIterator last, Compare comp = DefaultComp{})
        {
            if(first == last)
                return last;

            ForwardIterator next = first;
            for(++next; next != last; first = next, (void)++next)
                if(comp(next, first))
                    return next;
            return next;
        }

        //! std::is_sorted
        //! Taken from https://github.com/gcc-mirror/gcc/blob/d9375e490072d1aae73a93949aa158fcd2a27018/libstdc%2B%2B-v3/include/bits/stl_algo.h#L3193
        template<typename ForwardIterator,
                 typename DefaultComp = std::less<void>,
                 typename Compare     = DefaultComp>
        constexpr bool
        is_sorted(ForwardIterator first, ForwardIterator last, Compare comp = DefaultComp{})
        {
            return cx::is_sorted_until(first, last, comp) == last;
        }
    }  // namespace cx
}  // namespace executionGraph