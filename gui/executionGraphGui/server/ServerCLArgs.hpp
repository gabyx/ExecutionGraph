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

namespace executionGraphGui
{
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
        std::path rootPath() { return args::get(m_rootPath); }
        const std::string& address() { return args::get(m_address); }
        unsigned short port() { return args::get(m_port); }
        std::size_t threads() { return args::get(m_threads); }

    private:
        args::ValueFlag<std::string> m_rootPath;  //!< Server root path.
        args::ValueFlag<std::string> m_address;   //!< Server address.
        args::ValueFlag<unsigned short> m_port;   //!< Server port.
        args::ValueFlag<std::size_t> m_threads;   //!< Number of threads used for async. operations.
    };
}  // namespace executionGraphGui

#endif
