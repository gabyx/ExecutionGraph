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

#ifndef executionGraph_common_Log_hpp
#define executionGraph_common_Log_hpp

#include <iostream>
#include <fmt/format.h>
#include "executionGraph/config/Config.hpp"

#ifndef EXECGRAPH_FORCE_MSGLOG_LEVEL
#    error "EXECGRAPH_FORCE_MSGLOG_LEVEL needs to be defined!"
#endif

#define EXECGRAPH_LOGLEVEL_TRACE 0
#define EXECGRAPH_LOGLEVEL_DEBUG 1
#define EXECGRAPH_LOGLEVEL_INFO 2
#define EXECGRAPH_LOGLEVEL_WARN 3
#define EXECGRAPH_LOGLEVEL_ERROR 4
#define EXECGRAPH_LOGLEVEL_FATAL 5

// To Concat EXECGRAPH_LOGLEVEL_ and EXECGRAPH_FORCE_MSGLOG_LEVEL
#define CONCATT(L) EXECGRAPH_LOGLEVEL_##L  // x and y will not be expanded, just pasted
#define CONCAT(L) CONCATT(L)               // x and y will be expanded before the call to STEP2
#define EXECGRAPH_LOGLEVEL_CURRENT CONCAT(EXECGRAPH_FORCE_MSGLOG_LEVEL)

#define EXECGRAPH_LOGMSG(LEVEL, ...)                        \
    if(CONCAT(LEVEL) <= EXECGRAPH_LOGLEVEL_CURRENT)         \
    {                                                       \
        std::cerr << fmt::format(__VA_ARGS__) << std::endl; \
    }

#define EXECGRAPH_LOGMSG_LEVEL(LEVEL, ...)                                     \
    if(CONCAT(LEVEL) <= EXECGRAPH_LOGLEVEL_CURRENT)                            \
    {                                                                          \
        std::cerr << "[" #LEVEL "] " << fmt::format(__VA_ARGS__) << std::endl; \
    }

#define EXECGRAPH_LOGMSG_CONT_LEVEL(LEVEL, ...)                   \
    if(CONCAT(LEVEL) <= EXECGRAPH_LOGLEVEL_CURRENT)               \
    {                                                             \
        std::cerr << "[" #LEVEL "] " << fmt::format(__VA_ARGS__); \
    }

// Undefine all log macros
#define EXECGRAPH_LOG_TRACE(...)
#define EXECGRAPH_LOG_TRACE_CONT(...)
#define EXECGRAPH_LOG_DEBUG(...)
#define EXECGRAPH_LOG_INFO(...)
#define EXECGRAPH_LOG_WARN(...)
#define EXECGRAPH_LOG_ERROR(...)
#define EXECGRAPH_LOG_FATAL(...)

// Define only those which are active!
#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_TRACE
#    undef EXECGRAPH_LOG_TRACE
#    define EXECGRAPH_LOG_TRACE(...) EXECGRAPH_LOGMSG_LEVEL(TRACE, __VA_ARGS__)
#    undef EXECGRAPH_LOG_TRACE_CONT
#    define EXECGRAPH_LOG_TRACE_CONT(...) EXECGRAPH_LOGMSG_CONT_LEVEL(TRACE, __VA_ARGS__)
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_DEBUG
#    undef EXECGRAPH_LOG_DEBUG
#    define EXECGRAPH_LOG_DEBUG(...) EXECGRAPH_LOGMSG_LEVEL(DEBUG, __VA_ARGS__)
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_INFO
#    undef EXECGRAPH_LOG_INFO
#    define EXECGRAPH_LOG_INFO(...) EXECGRAPH_LOGMSG_LEVEL(INFO, __VA_ARGS__)
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_WARN
#    undef EXECGRAPH_LOG_WARN
#    define EXECGRAPH_LOG_WARN(...) EXECGRAPH_LOGMSG_LEVEL(WARN, __VA_ARGS__)
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_ERROR
#    undef EXECGRAPH_LOG_ERROR
#    define EXECGRAPH_LOG_ERROR(...) EXECGRAPH_LOGMSG_LEVEL(ERROR, __VA_ARGS__)
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_FATAL
#    undef EXECGRAPH_LOG_FATAL
#    define EXECGRAPH_LOG_FATAL(...) EXECGRAPH_LOGMSG_LEVEL(FATAL, __VA_ARGS__)
#endif

#undef CONCAT1
#undef CONCAT2

#endif
