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

#ifndef executionGraph_common_MetaVisit_hpp
#define executionGraph_common_MetaVisit_hpp

#include <meta/meta.hpp>
#include "executionGraph/common/Exception.hpp"

namespace meta
{
    namespace details
    {
        template<typename Func, typename List>
        struct visitTraits;

        template<typename Func, typename... Args>
        struct visitTraits<Func, meta::list<Args...>>
        {
            using MemberPtr = decltype(&Func::template invoke<void>);

            static constexpr std::array<MemberPtr, sizeof...(Args)> makeMap()
            {
                return {&Func::template invoke<Args>...};  // Build member function pointers.
            }
        };
    }  // namespace details

    //! Dispatch to the function `f.invoke<T>(args...)` where `T` is the type at index `index`.
    //! Throw exception if index is out of range!
    template<typename List, typename Func, typename... Args>
    void visit(Func&& f, std::size_t index, Args&&... args)
    {
        constexpr auto map = details::visitTraits<std::decay_t<Func>, List>::makeMap();
        EXECGRAPH_THROW_IF(index >= map.size(), "Index {0} >= {1} out of range!", index, map.size());
        (f.*map[index])(std::forward<Args>(args)...);
    }
}  // namespace meta

#endif