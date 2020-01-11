// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Jan 11 2020
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <string_view>

namespace executionGraph
{
    template<typename Char, Char... Cs>
    struct CharSequence
    {
    public:
        constexpr operator std::string_view() { return m_string; }

    private:
        static constexpr const Char m_string[] = {Cs..., 0};  //!< The unique address
    };

#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wpedantic"
#elif defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpedantic"
#endif

    // String literal operator to make a CharSequence
    template<typename Char, Char... Cs>
    constexpr CharSequence<Char, Cs...> operator"" _cs()
    {
        return {};
    }

#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(__GNUC__)
#    pragma GCC diagnostic pop
#endif

#undef COMPILER

}  // namespace executionGraph