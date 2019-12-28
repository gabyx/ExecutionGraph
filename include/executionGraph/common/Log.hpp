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

#include <iostream>
#include <fmt/format.h>
#include "executionGraph/config/Config.hpp"

#ifndef EG_FORCE_MSGLOG_LEVEL
#    error "EG_FORCE_MSGLOG_LEVEL needs to be defined!"
#endif

#define EG_LOGLEVEL_TRACE 0
#define EG_LOGLEVEL_DEBUG 1
#define EG_LOGLEVEL_INFO 2
#define EG_LOGLEVEL_WARN 3
#define EG_LOGLEVEL_ERROR 4
#define EG_LOGLEVEL_FATAL 5

// To Concat EG_LOGLEVEL_ and EG_FORCE_MSGLOG_LEVEL
#define CONCATT(L) EG_LOGLEVEL_##L  // x and y will not be expanded, just pasted
#define CONCAT(L) CONCATT(L)               // x and y will be expanded before the call to STEP2
#define EG_LOGLEVEL_CURRENT CONCAT(EG_FORCE_MSGLOG_LEVEL)

#define EG_LOGMSG(LEVEL, ...)                        \
    if(CONCAT(LEVEL) <= EG_LOGLEVEL_CURRENT)         \
    {                                                       \
        std::cerr << fmt::format(__VA_ARGS__) << std::endl; \
    }

#define EG_LOGMSG_LEVEL(LEVEL, ...)                                     \
    if(CONCAT(LEVEL) <= EG_LOGLEVEL_CURRENT)                            \
    {                                                                          \
        std::cerr << "[" #LEVEL "] " << fmt::format(__VA_ARGS__) << std::endl; \
    }

#define EG_LOGMSG_CONT_LEVEL(LEVEL, ...)                   \
    if(CONCAT(LEVEL) <= EG_LOGLEVEL_CURRENT)               \
    {                                                             \
        std::cerr << "[" #LEVEL "] " << fmt::format(__VA_ARGS__); \
    }

// Undefine all log macros
#define EG_LOG_TRACE(...)
#define EG_LOG_TRACE_CONT(...)
#define EG_LOG_DEBUG(...)
#define EG_LOG_INFO(...)
#define EG_LOG_WARN(...)
#define EG_LOG_ERROR(...)
#define EG_LOG_FATAL(...)

// Define only those which are active!
#if EG_LOGLEVEL_CURRENT <= EG_LOGLEVEL_TRACE
#    undef EG_LOG_TRACE
#    define EG_LOG_TRACE(...) EG_LOGMSG_LEVEL(TRACE, __VA_ARGS__)
#    undef EG_LOG_TRACE_CONT
#    define EG_LOG_TRACE_CONT(...) EG_LOGMSG_CONT_LEVEL(TRACE, __VA_ARGS__)
#endif

#if EG_LOGLEVEL_CURRENT <= EG_LOGLEVEL_DEBUG
#    undef EG_LOG_DEBUG
#    define EG_LOG_DEBUG(...) EG_LOGMSG_LEVEL(DEBUG, __VA_ARGS__)
#endif

#if EG_LOGLEVEL_CURRENT <= EG_LOGLEVEL_INFO
#    undef EG_LOG_INFO
#    define EG_LOG_INFO(...) EG_LOGMSG_LEVEL(INFO, __VA_ARGS__)
#endif

#if EG_LOGLEVEL_CURRENT <= EG_LOGLEVEL_WARN
#    undef EG_LOG_WARN
#    define EG_LOG_WARN(...) EG_LOGMSG_LEVEL(WARN, __VA_ARGS__)
#endif

#if EG_LOGLEVEL_CURRENT <= EG_LOGLEVEL_ERROR
#    undef EG_LOG_ERROR
#    define EG_LOG_ERROR(...) EG_LOGMSG_LEVEL(ERROR, __VA_ARGS__)
#endif

#if EG_LOGLEVEL_CURRENT <= EG_LOGLEVEL_FATAL
#    undef EG_LOG_FATAL
#    define EG_LOG_FATAL(...) EG_LOGMSG_LEVEL(FATAL, __VA_ARGS__)
#endif

#undef CONCAT1
#undef CONCAT2
