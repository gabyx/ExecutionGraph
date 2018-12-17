//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Tue Jan 09 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#include "executionGraphGui/server/ServerCLArgs.hpp"
#include <cstdlib>
#include <iostream>
#include <regex>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include "executionGraphGui/common/Exception.hpp"

ServerCLArgs::ServerCLArgs(int argc, const char* argv[])
    : executionGraph::CommandLineArguments(argc,
                                           argv,
                                           "ExecutionGraphServer Application",
                                           "No detailed description")
    , m_rootPath(m_parser,
                 "root",
                 "Root path of the server application.",
                 {'r', "rootPath"},
                 this->initialPath())
    , m_address(m_parser,
                "address",
                "The IP address to use.",
                {'a', "address"},
                "127.0.0.1")
    , m_port(m_parser,
             "port",
             "The port to use.",
             {'p', "port"},
             8089)
    , m_threads(m_parser,
                "threads",
                "The number of threads to use.",
                {'t', "threads"},
                2)
    , m_logPath(m_parser,
                "logPath",
                "Where the logs are placed.",
                {'l', "logPath"},
                this->initialPath() / "logs")

{
    args::HelpFlag help(m_parser, "help", "Display this help menu.", {'h', "help"});

    try
    {
        m_parser.ParseCLI(argc, argv);

        // Validation.
        m_threads.Get() = std::max(m_threads.Get(), {1});

        // Check address.
        std::regex ip("(?:(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\\.){3}(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])");
        EXECGRAPHGUI_THROW_TYPE_IF(!std::regex_match(m_address.Get(), ip),
                                   args::ParseError,
                                   "IP Address '{0}' has wrong format!",
                                   m_address.Get());

        // Adjust root path if relative.
        if(m_rootPath.Get().is_relative())
        {
            m_rootPath.Get() = this->initialPath() / m_rootPath.Get();
        }
    }
    catch(args::Help)
    {
        std::cerr << m_parser;
        std::exit(EXIT_SUCCESS);
    }
    catch(args::ParseError e)
    {
        std::cerr << fmt::format("Parser Error: '{0}'\n{1}",
                                 e.what(),
                                 m_parser);
        std::exit(EXIT_FAILURE);
    }
}
