// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <nuetzig (at) imes (d0t) mavt (d0t) ethz
//  (døt) ch>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_common_Asserts_hpp
#define ExecutionGraph_common_Asserts_hpp

#include <iostream>
#include <stdlib.h>
#include <typeinfo>

#include "ExecutionGraph/config/Config.hpp"
#include ExecutionGraph_Exception_INCLUDE_FILE

/**
* @brief An Assert Macro to use within C++ code.
* @param condition The condition which needs to be truem otherwise an assertion
* is thrown!
*/

#ifdef NDEBUG
    #define EXEC_GRAPH_ASSERTMSG(condition, message) 
#else
// Debug!
    #define EXEC_GRAPH_ASSERTMSG(condition, message) \
    {                                            \
        if (!(condition))                        \
        {                                        \
            ExecutionGraph_ERRORMSG(message)     \
        }                                        \
    }
#endif

#define EXEC_GRAPH_WARNINGMSG(condition, message)                                   \
    {                                                                               \
        if (!(condition))                                                           \
        {                                                                           \
            std::cerr << "WARNING: " << #condition << " : " << std::endl            \
                      << message << std::endl                                       \
                      << " @ " << __FILE__ << " (" << __LINE__ << ")" << std::endl; \
        }                                                                           \
    }
#define EXEC_GRAPH_ERRORMSG(message) EXEC_GRAPH_THROWEXCEPTION(message)

#endif
