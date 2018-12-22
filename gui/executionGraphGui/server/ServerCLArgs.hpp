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

#ifndef executionGraphGui_server_ServerCLArgs_hpp
#define executionGraphGui_server_ServerCLArgs_hpp

#include <executionGraph/common/CommandLineArguments.hpp>
#include <executionGraph/common/FileSystem.hpp>
#include <executionGraph/common/Singleton.hpp>

/* ---------------------------------------------------------------------------------------*/
/*!
        Commandline Arguments for the Server Application
        No need to make it thread-safe, since everything is read only!

        @date Fr Dez 14 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
/* ---------------------------------------------------------------------------------------*/
class ServerCLArgs final : public executionGraph::CommandLineArguments,
                           public executionGraph::Singleton<ServerCLArgs>
{
public:
    ServerCLArgs(int argc, const char* argv[]);
    virtual ~ServerCLArgs() = default;

public:
    const std::path& rootPath() { return m_rootPath.Get(); }
    const std::string& address() { return m_address.Get(); }
    unsigned short port() { return m_port.Get(); }
    std::size_t threads() { return m_threads.Get(); }
    const std::string& logPath() { return m_logPath.Get(); }

private:
    args::ValueFlag<std::path> m_rootPath;  //!< Server root path.
    args::ValueFlag<std::string> m_address;   //!< Server address.
    args::ValueFlag<unsigned short> m_port;   //!< Server port.
    args::ValueFlag<std::size_t> m_threads;   //!< Number of threads used for async. operations.
    args::ValueFlag<std::string> m_logPath;   //!< Server log path.
};

#endif
