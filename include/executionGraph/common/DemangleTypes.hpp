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

#ifndef executionGraph_common_DemangleTypes_hpp
#define executionGraph_common_DemangleTypes_hpp

#include <string>
#include <typeinfo>
#include "executionGraph/common/TypeDefs.hpp"

namespace executionGraph
{
    namespace details
    {
        EXECGRAPH_EXPORT std::string demangle(const char* name);
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

    EXECGRAPH_EXPORT std::string shortenTemplateBrackets(std::string s, unsigned int fromLevel = 1);
}  // namespace executionGraph

#endif
