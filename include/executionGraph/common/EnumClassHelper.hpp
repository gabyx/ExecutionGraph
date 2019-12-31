//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 08 2018
//!  @author Gabriel Nützi, <gnuetzi (at) gmail (døt) com>
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#pragma once

#include <type_traits>

namespace executionGraph
{
    /** This function casts any enum class to the underlying type */
    template<typename E>
    static constexpr auto enumToInt(E e) -> typename std::underlying_type<E>::type
    {
        return static_cast<typename std::underlying_type<E>::type>(e);
    }

    //! @todo P1073R3: consteval not implemented yet, so enumToIntC(E e)
    //! -> does not compile since template `integral_constan`
    //! needs a constexpr as second parameter.
    template<auto e>
    using enumToIntC = std::integral_constant<typename std::underlying_type<decltype(e)>::type, enumToInt(e)>;

}  // namespace executionGraph

// EnumClassHelper_hpp
