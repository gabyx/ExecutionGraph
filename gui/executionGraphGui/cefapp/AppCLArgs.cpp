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

#include "cefapp/AppCLArgs.hpp"
#include <cstdlib>
#include <iostream>
#include "executionGraph/common/Exception.hpp"
#include "executionGraph/common/StringFormat.hpp"

AppCLArgs::AppCLArgs(int argc, char* argv[])
    : executionGraph::CommandLineArguments(argc, argv, "ExecutionGUI Application", "No detailed description")
    , m_clientSourcePath(m_parser,
                         "client",
                         "Source path to the client application.",
                         {'c', "clientSourcePath"},
                         this->getApplicationPath().parent_path().append("/../client"))
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
        for(std::string& arg : m_cefArgsSplitted)
        {
            m_cefArgsArray.emplace_back(std::strlen(arg.c_str()));
            std::strcpy(m_cefArgsArray.back().data(), arg.c_str());
        }
    }
    catch(args::Help)
    {
        std::cout << m_parser;
        std::exit(EXIT_SUCCESS);
    }
    catch(args::ParseError e)
    {
        EXECGRAPH_THROW_EXCEPTION("Parser Error: " << e.what() << std::endl
                                                   << m_parser)
    }
}

//! Return CEF Arguments inform of an char*[] array.
//!
//! @return  Values stay valid as long as this singelton exists.
//!
std::vector<char*> AppCLArgs::getCEFArgs()
{
    std::vector<char*> vec(m_cefArgsArray.size() + 1);
    vec[0] = m_argv[0];
    for(auto charVec : m_cefArgsArray)
    {
        vec.emplace_back(charVec.data());
    }
    std::cout << vec.size() << std::endl;
    for(auto& v : vec)
    {
        std::cout << vec.back() << std::endl;
    }
    return vec;
}
