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

#include "executionGraph/config/Config.hpp"

#ifndef EXECGRAPH_FORCE_MSGLOG_LEVEL
#error "EXECGRAPH_FORCE_MSGLOG_LEVEL needs to be defined!"
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

#define EXECGRAPH_LOGMSG_LEVEL(LEVEL, MSG, END)     \
    if(CONCAT(LEVEL) <= EXECGRAPH_LOGLEVEL_CURRENT) \
    {                                               \
        std::cerr << "[" #LEVEL "] " << MSG END     \
    }

// Undefine all log macros
#define EXECGRAPH_LOG_TRACE(MSG)
#define EXECGRAPH_LOG_DEBUG(MSG)
#define EXECGRAPH_LOG_INFO(MSG)
#define EXECGRAPH_LOG_WARN(MSG)
#define EXECGRAPH_LOG_ERROR(MSG)
#define EXECGRAPH_LOG_FATAL(MSG)

// Define only those which are active!
#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_TRACE
#undef EXECGRAPH_LOG_TRACE
#define EXECGRAPH_LOG_TRACE(MSG) EXECGRAPH_LOGMSG_LEVEL(TRACE, MSG, << std::endl;)
#define EXECGRAPH_LOG_TRACE_NE(MSG) EXECGRAPH_LOGMSG_LEVEL(TRACE, MSG, )
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_DEBUG
#undef EXECGRAPH_LOG_DEBUG
#define EXECGRAPH_LOG_DEBUG(MSG) EXECGRAPH_LOGMSG_LEVEL(DEBUG, MSG, << std::endl;)
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_INFO
#undef EXECGRAPH_LOG_INFO
#define EXECGRAPH_LOG_INFO(MSG) EXECGRAPH_LOGMSG_LEVEL(INFO, MSG, << std::endl;)
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_WARN
#undef EXECGRAPH_LOG_WARN
#define EXECGRAPH_LOG_WARN(MSG) EXECGRAPH_LOGMSG_LEVEL(WARN, MSG, << std::endl;)
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_ERROR
#undef EXECGRAPH_LOG_ERROR
#define EXECGRAPH_LOG_ERROR(MSG) EXECGRAPH_LOGMSG_LEVEL(ERROR, MSG, << std::endl;)
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_FATAL
#undef EXECGRAPH_LOG_FATAL
#define EXECGRAPH_LOG_FATAL(MSG) EXECGRAPH_LOGMSG_LEVEL(FATAL, MSG, << std::endl;)
#endif

#undef CONCAT1
#undef CONCAT2

#endif
