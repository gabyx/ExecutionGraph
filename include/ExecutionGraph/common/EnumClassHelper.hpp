// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <nuetzig (at) imes (d0t) mavt (d0t) ethz
//  (døt) ch>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_common_EnumClassHelper_hpp
#define ExecutionGraph_common_EnumClassHelper_hpp

#include <type_traits>

namespace executionGraph
{
/** This function casts any enum class to the underlying type */
template<typename E>
static constexpr auto enumToInt(const E e) -> typename std::underlying_type<E>::type
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}

template<typename E>
static constexpr auto enumToIntC(const E e) -> typename std::underlying_type<E>::type
{
    return std::integral_constant<typename std::underlying_type<E>::type, enumToInt(e)>{};
}
}

#endif  // EnumClassHelper_hpp
