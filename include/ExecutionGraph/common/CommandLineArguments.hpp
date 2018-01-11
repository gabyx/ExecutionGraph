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

#ifndef ExecutionGraph_common_CommandLineArguments_hpp
#define ExecutionGraph_common_CommandLineArguments_hpp

#include <args.hxx>
#include "ExecutionGraph/common/FileSystem.hpp"

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
            , m_applicationPath(argv[0])
        {
        }
        virtual ~CommandLineArguments() = default;

    public:
        //! Get the application path. (can be relative)
        const std::path& getApplicationPath() { return m_applicationPath; }

    private:
        std::path m_applicationPath;  //!< Application path: argv[0]

    protected:
        args::ArgumentParser m_parser;
    };
}

#endif