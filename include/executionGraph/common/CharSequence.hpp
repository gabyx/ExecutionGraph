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

namespace executionGraph
{
    template<typename Char, Char... Cs>
    struct CharSequence
    {
    public:
        static constexpr std::string_view str() const { return m_string; }

    private:
        static constexpr const String m_string[] = {Cs..., 0};  //!< The unique address
    };

    // String literal operator to make a CharSequence
    template<typename Char, Char... Cs>
    constexpr CharSequence<Char, Cs...> operator"" _cs()
    {
        return {};
    }

}  // namespace executionGraph