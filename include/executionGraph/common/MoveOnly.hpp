// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Apr 22 2020
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

namespace executionGraph
{
    //! Move-only class serving as a base 
    //! class to protect classes from copying
    class MoveOnly
    {
    public:
        MoveOnly()  = default;
        ~MoveOnly() = default;

        MoveOnly(MoveOnly&&) = default;
        MoveOnly& operator=(MoveOnly&&) = default;

        MoveOnly(MoveOnly const&) = delete;
        MoveOnly& operator=(MoveOnly const&) = delete;
    };
}  // namespace executionGraph