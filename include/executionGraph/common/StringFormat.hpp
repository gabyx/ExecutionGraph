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

#ifndef executionGraph_common_StringFormat_hpp
#define executionGraph_common_StringFormat_hpp

#include <cstdio>
#include <memory>
#include <string>

namespace executionGraph
{
    //! Format a string like printf, http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
    template<typename... Args>
    std::string stringFormat(const std::string& format, Args... args)
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"

        std::size_t size = (std::size_t)snprintf(nullptr, 0, format.c_str(), args...) + 1;  // Extra space for '\0'
        std::unique_ptr<char[]> buf(new char[size]);
        snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1);  // We don't want the '\0' inside

#pragma clang diagnostic pop
    }
}

#endif
