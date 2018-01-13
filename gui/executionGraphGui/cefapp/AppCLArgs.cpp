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
#include "ExecutionGraph/common/Exception.hpp"

AppCLArgs::AppCLArgs(int argc, char* argv[])
    : executionGraph::CommandLineArguments(argc, argv, "ExecutionGUI Application", "No detailed description")
    , m_clientSourcePath(m_parser,
                         "client",
                         "Source path to the client application.",
                         {'c', "clientSourcePath"},
                         this->getApplicationPath().parent_path().append("/../client"))
{
    args::HelpFlag help(m_parser, "help", "Display this help menu.", {'h', "help"});

    try
    {
        m_parser.ParseCLI(argc, argv);
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
