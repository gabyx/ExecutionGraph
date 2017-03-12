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

namespace ExecutionGraph
{
/** This function casts any enum class to the underlying type */
template <typename E>
constexpr auto toIntegral(const E e) -> typename std::underlying_type<E>::type
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}
};

#endif  // EnumClassHelper_hpp
