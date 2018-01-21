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

#ifndef cefapp_AppCLArgs_hpp
#define cefapp_AppCLArgs_hpp

#include <ExecutionGraph/common/CommandLineArguments.hpp>
#include <ExecutionGraph/common/Singleton.hpp>
#include "executionGraph/common/FileSystem.hpp"

//! Commandline Arguments for the Application
//! No need to make it thread-safe, since everything is read only!
class AppCLArgs final : public executionGraph::CommandLineArguments,
                        public executionGraph::Singleton<AppCLArgs>
{
public:
    AppCLArgs(int argc, char* argv[]);
    virtual ~AppCLArgs() = default;

    //! @name Client Application Arguments
    //@{
public:
    std::path getClientSourcePath() { return args::get(m_clientSourcePath); }

private:
    args::ValueFlag<std::string> m_clientSourcePath;
    //@}
};

#endif
