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

#include "executionGraphGui/cefapp/AppCLArgs.hpp"
#include <cstdlib>
#include <iostream>
#include <executionGraph/common/StringFormat.hpp>
#include "executionGraphGui/common/Exception.hpp"
#include "executionGraphGui/common/Loggers.hpp"

AppCLArgs::AppCLArgs(int argc, char* argv[])
    : executionGraph::CommandLineArguments(argc, argv, "ExecutionGUI Application", "No detailed description")
    , m_clientSourcePath(m_parser,
                         "client",
                         "Source path to the client application.",
                         {'c', "clientSourcePath"},
                         this->getApplicationPath().parent_path().append("/../client/apps/eg"))
    , m_clientLoadUrl(m_parser,
                      "clientUrl",
                      "Url which is loaded at startup.",
                      {'u', "clientLoadUrl"},
                      "client://executiongraph/index.html")
    , m_logPath(m_parser,
                "logPath",
                "Directory path where all logs are placed.",
                {'l', "logPath"},
                this->getApplicationPath().parent_path().append("/../"))
    , m_cefArgs(m_parser,
                "cefFlags",
                "All flags submitted to CEF (one string)",
                {'f', "cefFlags"},
                "")
{
    args::HelpFlag help(m_parser, "help", "Display this help menu.", {'h', "help"});

    try
    {
        m_parser.ParseCLI(argc, argv);

        // Post process CEF args
        m_cefArgsSplitted = executionGraph::splitString(args::get(m_cefArgs));

        m_cefArgsArray.reserve(1 + m_cefArgsSplitted.size());  // No reallocation!
        m_cefArgsArray.emplace_back(std::strlen(argv[0]) + 1);
        std::strcpy(m_cefArgsArray.back().data(), argv[0]);
        m_cefArgsPtrArray.emplace_back(m_cefArgsArray.back().data());

        for(std::string& arg : m_cefArgsSplitted)
        {
            std::cout << "CEF Arg: " << arg << std::endl;
            m_cefArgsArray.emplace_back(arg.size() + 1);
            std::strcpy(m_cefArgsArray.back().data(), arg.c_str());
            m_cefArgsPtrArray.emplace_back(m_cefArgsArray.back().data());
        }
    }
    catch(args::Help)
    {
        std::cout << m_parser;
        std::exit(EXIT_SUCCESS);
    }
    catch(args::ParseError e)
    {
        EGGUI_THROW("Parser Error: '{0}'\n{1}", e.what(), m_parser);
    }
}

//! Return CEF Arguments inform of an char*[] array.
//!
//! @return  Values stay valid as long as this singleton exists.
//!
std::vector<char*>& AppCLArgs::getCEFArgs()
{
    return m_cefArgsPtrArray;
}
