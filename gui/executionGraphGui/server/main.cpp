// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Fri Nov 30 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include "executionGraphGui/common/Loggers.hpp"
#include "executionGraphGui/server/HttpCommon.hpp"
#include "executionGraphGui/server/HttpListener.hpp"
#include "executionGraphGui/server/ServerCLArgs.hpp"

//! Main function of the execution graph server backend.
int main(int argc, const char* argv[])
{
    // Parse command line arguments
    EXECGRAPH_INSTANCIATE_SINGLETON_CTOR(ServerCLArgs, args, (argc, argv));

    // Make all loggers
    EXECGRAPH_INSTANCIATE_SINGLETON_CTOR(Loggers, loggers, (args->logPath()));

    EXECGRAPHGUI_BACKENDLOG_INFO(
        "Starting ExecutionGraph Server at '{0}:{1} with '{2}' threads.\n"
        "Logs located at '{3}'",
        args->address(),
        args->port(),
        args->threads(),
        args->logPath());

    // The io_context is required for all I/O
    const auto threads = args->threads();
    boost::asio::io_context ioc{static_cast<int>(threads)};

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
        [&](boost::system::error_code const&, int) {
            EXECGRAPHGUI_BACKENDLOG_INFO("ExecutionGraph Server shutting down ...");
            // Stop the `io_context`. This will cause `run()`
            // to return immediately, eventually destroying the
            // `io_context` and all of the sockets in it.
            ioc.stop();
        });

    // Create and launch a listening port
    const auto address = boost::asio::ip::make_address(args->address());
    std::make_shared<HttpListener>(
        ioc,
        boost::asio::ip::tcp::endpoint{address, args->port()},
        args->rootPath())
        ->run();

    auto run = [&ioc](auto threadIdx) {
        EXECGRAPHGUI_BACKENDLOG_INFO("Start thread '{0}' ...", threadIdx);
        ioc.run();
        EXECGRAPHGUI_BACKENDLOG_INFO("End thread '{0}' ...", threadIdx);
    };

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> ths;
    ths.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
    {
        ths.emplace_back(std::bind(run, i));
    }
    run(0);

    // If we get here, it means we got a SIGINT or SIGTERM
    // Block until all the threads exit
    for(auto& th : ths)
    {
        th.join();
    }

    EXECGRAPHGUI_BACKENDLOG_INFO("ExecutionGraph Server shutdown.");
    return EXIT_SUCCESS;
}