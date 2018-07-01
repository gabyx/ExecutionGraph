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

#include "common/Loggers.hpp"

Loggers::Loggers(const std::path& logPath)
{
    auto format  = "[%H:%M:%S,%e] [%l] [%n] [tid: %t] : %v";
    auto logFile = logPath / "AppLog.log";

    try
    {
        auto stdOutSink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
        auto fileSink   = std::make_shared<spdlog::sinks::simple_file_sink_mt>(logFile.string());

        std::vector<spdlog::sink_ptr> sinks{stdOutSink, fileSink};

        // make the application log
        m_appLog = std::make_unique<spdlog::logger>("AppLog", begin(sinks), end(sinks));
        m_appLog->set_level(spdlog::level::debug);
        m_appLog->set_pattern(format);

        // make the backend log
        m_backendLog = std::make_unique<spdlog::logger>("BackendLog", begin(sinks), end(sinks));
        m_backendLog->set_level(spdlog::level::debug);
        m_backendLog->set_pattern(format);
    }
    catch(const spdlog::spdlog_ex& ex)
    {
        EXECGRAPH_THROW_EXCEPTION("Log initialization failed: " << ex.what());
    }
}