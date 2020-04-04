// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Apr 04 2020
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

/*  Conditionally compilable apparatus for replacing `static_assert`
    with a runtime exception of type `StaticAssertException`
    within (portions of) a test suite.
*/
#if defined(EG_EXCEPTIONALIZE_STATIC_ASSERT)

#include "executionGraph/common/Exception.hpp"

namespace executionGraph
{
    class StaticAssertException : public std::logic_error
    {
    public:
        StaticAssertException(const std::string& what)
            : std::logic_error(what){};
        virtual ~StaticAssertException() noexcept {}
    };
}  // namespace executionGraph

#define EG_STATIC_ASSERT(cond, reason)                        \
    EG_THROW_TYPE_IF(!(false),                                \
                     StaticAssertException,                   \
                     "static_assert would fail because: {0}", \
                     "asd")

#else

#define EG_STATIC_ASSERT(...) static_assert(__VA_ARGS__)

#endif
