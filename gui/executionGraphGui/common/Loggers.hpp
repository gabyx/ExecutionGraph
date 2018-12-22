//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Fri Feb 09 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraphGui_common_Logger_hpp
#define executionGraphGui_common_Logger_hpp

#include <spdlog/spdlog.h>
#include <executionGraph/common/FileSystem.hpp>
#include <executionGraph/common/Log.hpp>
#include <executionGraph/common/Singleton.hpp>

// Undef all macros
#define EXECGRAPHGUI_APPLOG_TRACE(...)
#define EXECGRAPHGUI_APPLOG_DEBUG(...)
#define EXECGRAPHGUI_APPLOG_INFO(...)
#define EXECGRAPHGUI_APPLOG_WARN(...)
#define EXECGRAPHGUI_APPLOG_ERROR(...)
#define EXECGRAPHGUI_APPLOG_FATAL(...)

#define EXECGRAPHGUI_BACKENDLOG_TRACE(...)
#define EXECGRAPHGUI_BACKENDLOG_DEBUG(...)
#define EXECGRAPHGUI_BACKENDLOG_INFO(...)
#define EXECGRAPHGUI_BACKENDLOG_WARN(...)
#define EXECGRAPHGUI_BACKENDLOG_ERROR(...)
#define EXECGRAPHGUI_BACKENDLOG_FATAL(...)

#define EXECGRAPHGUI_LOGCODE_TRACE(...)
#define EXECGRAPHGUI_LOGCODE_DEBUG(...)
#define EXECGRAPHGUI_LOGCODE_INFO(...)
#define EXECGRAPHGUI_LOGCODE_WARN(...)
#define EXECGRAPHGUI_LOGCODE_ERROR(...)
#define EXECGRAPHGUI_LOGCODE_FATAL(...)

// Define only those which are active!
#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_TRACE
#    undef EXECGRAPHGUI_APPLOG_TRACE
#    define EXECGRAPHGUI_APPLOG_TRACE(...) Loggers::getInstance().getAppLogger().trace(__VA_ARGS__)
#    undef EXECGRAPHGUI_BACKENDLOG_TRACE
#    define EXECGRAPHGUI_BACKENDLOG_TRACE(...) Loggers::getInstance().getBackendLogger().trace(__VA_ARGS__)
#    undef EXECGRAPHGUI_LOGCODE_TRACE
#    define EXECGRAPHGUI_LOGCODE_TRACE(expr) expr
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_DEBUG
#    undef EXECGRAPHGUI_APPLOG_DEBUG
#    define EXECGRAPHGUI_APPLOG_DEBUG(...) Loggers::getInstance().getAppLogger().debug(__VA_ARGS__)
#    undef EXECGRAPHGUI_BACKENDLOG_DEBUG
#    define EXECGRAPHGUI_BACKENDLOG_DEBUG(...) Loggers::getInstance().getBackendLogger().debug(__VA_ARGS__)
#    undef EXECGRAPHGUI_LOGCODE_DEBUG
#    define EXECGRAPHGUI_LOGCODE_DEBUG(expr) expr
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_INFO
#    undef EXECGRAPHGUI_APPLOG_INFO
#    define EXECGRAPHGUI_APPLOG_INFO(...) Loggers::getInstance().getAppLogger().info(__VA_ARGS__)
#    undef EXECGRAPHGUI_BACKENDLOG_INFO
#    define EXECGRAPHGUI_BACKENDLOG_INFO(...) Loggers::getInstance().getBackendLogger().info(__VA_ARGS__)
#    undef EXECGRAPHGUI_LOGCODE_INFO
#    define EXECGRAPHGUI_LOGCODE_INFO(expr) expr
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_WARN
#    undef EXECGRAPHGUI_APPLOG_WARN
#    define EXECGRAPHGUI_APPLOG_WARN(...) Loggers::getInstance().getAppLogger().warn(__VA_ARGS__)
#    undef EXECGRAPHGUI_BACKENDLOG_WARN
#    define EXECGRAPHGUI_BACKENDLOG_WARN(...) Loggers::getInstance().getBackendLogger().warn(__VA_ARGS__)
#    undef EXECGRAPHGUI_LOGCODE_WARN
#    define EXECGRAPHGUI_LOGCODE_WARN(expr) expr
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_ERROR
#    undef EXECGRAPHGUI_APPLOG_ERROR
#    define EXECGRAPHGUI_APPLOG_ERROR(...) Loggers::getInstance().getAppLogger().error(__VA_ARGS__)
#    undef EXECGRAPHGUI_BACKENDLOG_ERROR
#    define EXECGRAPHGUI_BACKENDLOG_ERROR(...) Loggers::getInstance().getBackendLogger().error(__VA_ARGS__)
#    undef EXECGRAPHGUI_LOGCODE_ERROR
#    define EXECGRAPHGUI_LOGCODE_ERROR(expr) expr
#endif

#if EXECGRAPH_LOGLEVEL_CURRENT <= EXECGRAPH_LOGLEVEL_FATAL
#    undef EXECGRAPHGUI_APPLOG_FATAL
#    define EXECGRAPHGUI_APPLOG_FATAL(...) Loggers::getInstance().getAppLogger().critical(__VA_ARGS__)
#    undef EXECGRAPHGUI_BACKENDLOG_FATAL
#    define EXECGRAPHGUI_BACKENDLOG_FATAL(...) Loggers::getInstance().getBackendLogger().critical(__VA_ARGS__)
#    undef EXECGRAPHGUI_LOGCODE_FATAL
#    define EXECGRAPHGUI_LOGCODE_FATAL(expr) expr
#endif

/* ---------------------------------------------------------------------------------------*/
/*!
    The logger storage for the application

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class Loggers : public executionGraph::Singleton<Loggers>
{
public:
    Loggers(const std::path& logPath = "./");
    ~Loggers() = default;

    spdlog::logger& getAppLogger() { return *m_appLog; }
    spdlog::logger& getBackendLogger() { return *m_backendLog; }

private:
    std::unique_ptr<spdlog::logger> m_backendLog;  //! Log for the backend (mutli-threaded)
    std::unique_ptr<spdlog::logger> m_appLog;      //! Log for the app (multi-threaded)
};

#endif
