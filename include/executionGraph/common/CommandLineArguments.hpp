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

#ifndef executionGraph_common_CommandLineArguments_hpp
#define executionGraph_common_CommandLineArguments_hpp

#include <args.hxx>
#include "executionGraph/common/FileSystem.hpp"
#include "executionGraph/config/Config.hpp"

namespace executionGraph
{
    class CommandLineArguments
    {
    public:
        CommandLineArguments(int argc,
                             char* argv[],
                             const std::string& briefDescription,
                             const std::string& detailedDescription)
            : m_parser(briefDescription, detailedDescription)
            , m_argc(argc)
            , m_argv(argv)
            , m_applicationPath(argv[0])
        {
        }
        virtual ~CommandLineArguments() = default;

    public:
        //! Get the application path. (can be relative)
        const std::path& getApplicationPath() { return m_applicationPath; }

    protected:
        args::ArgumentParser m_parser;  //! Argument parser

        int m_argc;
        char** m_argv;

    private:
        std::path m_applicationPath;  //!< Application path: argv[0]
    };
}  // namespace executionGraph

#endif