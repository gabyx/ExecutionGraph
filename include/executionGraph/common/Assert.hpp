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

#pragma once

#include "executionGraph/common/Exception.hpp"
#include "executionGraph/common/Log.hpp"
#include "executionGraph/config/Config.hpp"

//! Some assert macro.
#ifdef NDEBUG
#    define EXECGRAPH_DEBUG_ONLY(code)
#    define EXECGRAPH_ASSERT(condition, ...)
#    define EXECGRAPH_VERIFY(condition, ...) condition;
#    define EXECGRAPH_ASSERT_TYPE(condition, type, ...)
#else
// Debug!
#    define EXECGRAPH_DEBUG_ONLY(code) code
#    define EXECGRAPH_VERIFY(condition, ...) EXECGRAPH_ASSERT(condition, __VA_ARGS__)
#    define EXECGRAPH_ASSERT(condition, ...) EXECGRAPH_ASSERT_TYPE(condition, executionGraph::ExceptionFatal, __VA_ARGS__)
#    define EXECGRAPH_ASSERT_TYPE(condition, Type, ...)           \
        if(!(condition))                                          \
        {                                                         \
            EXECGRAPH_LOG_ERROR("{0} : \n{1}\n@ {2} [{3}]",       \
                                #condition,                       \
                                fmt::format(__VA_ARGS__),         \
                                __FILE__,                         \
                                __LINE__)                         \
            EXECGRAPH_THROW_TYPE(!(condition), Type, __VA_ARGS__) \
        }
#endif

//! Some warning macro.
#define EXECGRAPH_WARN(condition, ...)                     \
    {                                                      \
        if(!(condition))                                   \
        {                                                  \
            EXECGRAPH_LOG_WARN("{0} : \n{1}\n@ {2} [{3}]", \
                               #condition,                 \
                               fmt::format(__VA_ARGS__),   \
                               __FILE__,                   \
                               __LINE__)                   \
        }                                                  \
    }
