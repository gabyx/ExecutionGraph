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

#include <executionGraph/common/CommandLineArguments.hpp>
#include <executionGraph/common/FileSystem.hpp>
#include <executionGraph/common/Singleton.hpp>

/* ---------------------------------------------------------------------------------------*/
/*!
    Commandline Arguments for the Application
    No need to make it thread-safe, since everything is read only!

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
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
    std::path getLogPath() { return args::get(m_logPath); }
    std::vector<char*> getCEFArgs();

private:
    args::ValueFlag<std::string> m_clientSourcePath;  //!< Path to the client source files (anuglar application)
    args::ValueFlag<std::string> m_logPath;           //!< Path where all logs are placed

    args::ValueFlag<std::string> m_cefArgs;      //!< All forwarded command line args to CEF.
    std::vector<std::string> m_cefArgsSplitted;  //!< Splitted CEF args.
    std::vector<std::vector<char>> m_cefArgsArray;
    //@}
};

#endif
