// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#include "ExecutionGraph/common/DemangleTypes.hpp"

#if defined (__GNUG__) || defined (__clang__)
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#endif

namespace executionGraph
{

namespace details
{
#if defined (__GNUG__) || defined (__clang__)
    std::string demangle(const char* name)
    {
        int status;
        std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};
        return (status == 0) ? res.get() : name;
    }
#else
    std::string demangle(const char* name)
    {
        return name;
    }
#endif
}

} // executionGraph
