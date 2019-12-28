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

#pragma once

#include <spdlog/spdlog.h>
#include <executionGraph/common/FileSystem.hpp>
#include <executionGraph/common/Log.hpp>
#include <executionGraph/common/Singleton.hpp>

// Undef all macros
#define EGGUI_APPLOG_TRACE(...)
#define EGGUI_APPLOG_DEBUG(...)
#define EGGUI_APPLOG_INFO(...)
#define EGGUI_APPLOG_WARN(...)
#define EGGUI_APPLOG_ERROR(...)
#define EGGUI_APPLOG_FATAL(...)

#define EGGUI_BACKENDLOG_TRACE(...)
#define EGGUI_BACKENDLOG_DEBUG(...)
#define EGGUI_BACKENDLOG_INFO(...)
#define EGGUI_BACKENDLOG_WARN(...)
#define EGGUI_BACKENDLOG_ERROR(...)
#define EGGUI_BACKENDLOG_FATAL(...)

#define EGGUI_LOGCODE_TRACE(...)
#define EGGUI_LOGCODE_DEBUG(...)
#define EGGUI_LOGCODE_INFO(...)
#define EGGUI_LOGCODE_WARN(...)
#define EGGUI_LOGCODE_ERROR(...)
#define EGGUI_LOGCODE_FATAL(...)

// Define only those which are active!
#if EG_LOGLEVEL_CURRENT <= EG_LOGLEVEL_TRACE
#    undef EGGUI_APPLOG_TRACE
#    define EGGUI_APPLOG_TRACE(...) Loggers::getInstance().getAppLogger().trace(__VA_ARGS__)
#    undef EGGUI_BACKENDLOG_TRACE
#    define EGGUI_BACKENDLOG_TRACE(...) Loggers::getInstance().getBackendLogger().trace(__VA_ARGS__)
#    undef EGGUI_LOGCODE_TRACE
#    define EGGUI_LOGCODE_TRACE(expr) expr
#endif

#if EG_LOGLEVEL_CURRENT <= EG_LOGLEVEL_DEBUG
#    undef EGGUI_APPLOG_DEBUG
#    define EGGUI_APPLOG_DEBUG(...) Loggers::getInstance().getAppLogger().debug(__VA_ARGS__)
#    undef EGGUI_BACKENDLOG_DEBUG
#    define EGGUI_BACKENDLOG_DEBUG(...) Loggers::getInstance().getBackendLogger().debug(__VA_ARGS__)
#    undef EGGUI_LOGCODE_DEBUG
#    define EGGUI_LOGCODE_DEBUG(expr) expr
#endif

#if EG_LOGLEVEL_CURRENT <= EG_LOGLEVEL_INFO
#    undef EGGUI_APPLOG_INFO
#    define EGGUI_APPLOG_INFO(...) Loggers::getInstance().getAppLogger().info(__VA_ARGS__)
#    undef EGGUI_BACKENDLOG_INFO
#    define EGGUI_BACKENDLOG_INFO(...) Loggers::getInstance().getBackendLogger().info(__VA_ARGS__)
#    undef EGGUI_LOGCODE_INFO
#    define EGGUI_LOGCODE_INFO(expr) expr
#endif

#if EG_LOGLEVEL_CURRENT <= EG_LOGLEVEL_WARN
#    undef EGGUI_APPLOG_WARN
#    define EGGUI_APPLOG_WARN(...) Loggers::getInstance().getAppLogger().warn(__VA_ARGS__)
#    undef EGGUI_BACKENDLOG_WARN
#    define EGGUI_BACKENDLOG_WARN(...) Loggers::getInstance().getBackendLogger().warn(__VA_ARGS__)
#    undef EGGUI_LOGCODE_WARN
#    define EGGUI_LOGCODE_WARN(expr) expr
#endif

#if EG_LOGLEVEL_CURRENT <= EG_LOGLEVEL_ERROR
#    undef EGGUI_APPLOG_ERROR
#    define EGGUI_APPLOG_ERROR(...) Loggers::getInstance().getAppLogger().error(__VA_ARGS__)
#    undef EGGUI_BACKENDLOG_ERROR
#    define EGGUI_BACKENDLOG_ERROR(...) Loggers::getInstance().getBackendLogger().error(__VA_ARGS__)
#    undef EGGUI_LOGCODE_ERROR
#    define EGGUI_LOGCODE_ERROR(expr) expr
#endif

#if EG_LOGLEVEL_CURRENT <= EG_LOGLEVEL_FATAL
#    undef EGGUI_APPLOG_FATAL
#    define EGGUI_APPLOG_FATAL(...) Loggers::getInstance().getAppLogger().critical(__VA_ARGS__)
#    undef EGGUI_BACKENDLOG_FATAL
#    define EGGUI_BACKENDLOG_FATAL(...) Loggers::getInstance().getBackendLogger().critical(__VA_ARGS__)
#    undef EGGUI_LOGCODE_FATAL
#    define EGGUI_LOGCODE_FATAL(expr) expr
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
