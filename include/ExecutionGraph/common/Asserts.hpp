// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
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

#include "ExecutionGraph/common/Exception.hpp"
#include "ExecutionGraph/config/Config.hpp"

//! Some assert macro.
#ifdef NDEBUG
#define EXEC_GRAPH_ASSERT(condition, message)
#define EXEC_GRAPH_ASSERT_TYPE(condition, message, type)
#else
// Debug!
#define EXEC_GRAPH_ASSERT(condition, message) EXEC_GRAPH_ASSERT_TYPE(condition, message, Exception)
#define EXEC_GRAPH_ASSERT_TYPE(condition, message, type)  \
    {                                                     \
        if (!(condition))                                 \
        {                                                 \
            EXEC_GRAPH_THROWEXCEPTION_TYPE(message, type) \
        }                                                 \
    }
#endif

//! Some warning macro.
#define EXEC_GRAPH_WARNINGMSG(condition, message)                                   \
    {                                                                               \
        if (!(condition))                                                           \
        {                                                                           \
            std::cerr << "WARNING: " << #condition << " : " << std::endl            \
                      << message << std::endl                                       \
                      << " @ " << __FILE__ << " (" << __LINE__ << ")" << std::endl; \
        }                                                                           \
    }
#endif
