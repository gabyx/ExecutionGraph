// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Sep 01 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGUI_common_Assert_hpp
#define executionGraphGUI_common_Assert_hpp

#include "executionGraphGUI/common/Loggers.hpp"

// Define some asserts
#define EXECGRAPH_STRINGIFY(x) #x
#define EXECGRAPH_TOSTRING(x) EXECGRAPH_STRINGIFY(x)

#ifdef NDEBUG
#    define EXECGRAPHGUI_ASSERT(condition, ...)                                                    \
        {                                                                                          \
            if(!(condition))                                                                       \
            {                                                                                      \
                EXECGRAPHGUI_APPLOG_FATAL(__VA_ARGS__);                                            \
                EXECGRAPHGUI_THROW("Exception: @ " __FILE__ "(" EXECGRAPH_TOSTRING(__LINE__) ")"); \
            }                                                                                      \
        }

#    define EXECGRAPHGUI_VERIFY(condition, ...) EXECGRAPHGUI_ASSERT(condition, __VA_ARGS__)
#else
#    define EXECGRAPHGUI_ASSERT(condition, ...)         \
        {                                               \
            if(!(condition))                            \
            {                                           \
                EXECGRAPHGUI_APPLOG_FATAL(__VA_ARGS__); \
            }                                           \
        }
#    define EXECGRAPHGUI_VERIFY(condition, ...) EXECGRAPHGUI_ASSERT(condition, __VA_ARGS__)
#endif

#endif