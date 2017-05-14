// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_common_DemangleTypes_hpp
#define ExecutionGraph_common_DemangleTypes_hpp

#include <string>
#include <typeinfo>

namespace executionGraph
{
namespace details
{
std::string demangle(const char* name);
}

template<typename T>
std::string demangle(T&& t)
{
    return details::demangle(typeid(t).name());
}

template<typename T>
std::string demangle()
{
    return details::demangle(typeid(T).name());
}

std::string shortenTemplateBrackets(std::string s, unsigned int fromLevel = 1);
}

#endif
