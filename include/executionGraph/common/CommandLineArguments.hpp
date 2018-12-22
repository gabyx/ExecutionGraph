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
                             const char* argv[],
                             const std::string& briefDescription,
                             const std::string& detailedDescription)
            : m_parser(briefDescription, detailedDescription)
            , m_argc(argc)
            , m_argv(argv)
            , m_applicationPath(argv[0])
            , m_initialPath(std::filesystem::current_path())
        {
            if(m_applicationPath.is_relative())
            {
                m_applicationPath = m_initialPath / m_applicationPath.filename();
            }
        }
        virtual ~CommandLineArguments() = default;

    public:
        //! Get the absolute application path.
        const std::path& applicationPath() { return m_applicationPath; }
        //! Get the absolute initial path.
        const std::path& initialPath() { return m_initialPath; }

    protected:
        args::ArgumentParser m_parser;  //!< Argument parser

        int m_argc;           //!< main() argument count.
        const char** m_argv;  //!< main() argument array.

    private:
        std::path m_applicationPath;  //!< Absolute application path from argv[0].
        std::path m_initialPath;      //!< Absolute initial path.
    };
}  // namespace executionGraph

#endif