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
#include <list>
#include <memory>
#include <thread>
#include <vector>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include "executionGraphGui/backend/BackendFactory.hpp"
#include "executionGraphGui/backend/ExecutionGraphBackend.hpp"
#include "executionGraphGui/common/Loggers.hpp"
#include "executionGraphGui/server/BackendRequestDispatcher.hpp"
#include "executionGraphGui/server/HttpCommon.hpp"
#include "executionGraphGui/server/HttpListener.hpp"
#include "executionGraphGui/server/HttpSession.hpp"
#include "executionGraphGui/server/HttpWorker.hpp"
#include "executionGraphGui/server/ServerCLArgs.hpp"

//! Runs all workers.
template<typename IOContext>
auto runWorkers(IOContext& ioc, std::size_t threads)
{
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

    auto run = [&ioc](auto threadIdx) {
        // If an exception is not handled and
        EXECGRAPHGUI_BACKENDLOG_INFO("Start thread '{0}' ...", threadIdx);
        try
        {
            ioc.run();  // Blocking!
        }
        catch(std::exception& e)
        {
            EXECGRAPHGUI_BACKENDLOG_FATAL("Stop Executor: Exception on thread '{0}' : '{1}'", threadIdx, e.what());
            ioc.stop();  // Non blocking!
        }
        EXECGRAPHGUI_BACKENDLOG_INFO("End thread '{0}' ...", threadIdx);
    };

    // Run the I/O service on the requested number of threads.
    std::vector<std::thread> ths;
    ths.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
    {
        ths.emplace_back(std::bind(run, i));
    }
    run(0); // Blocking

    return ths;
}

//! Install various backends and setup all of them.
template<typename Dispatcher>
void setupBackends(std::shared_ptr<Dispatcher> requestDispatcher)
{
    // Install the executionGraph backend
    BackendFactory::BackendData messageHandlers = BackendFactory::Create<ExecutionGraphBackend>();
    for(auto& backendHandler : messageHandlers.second)
    {
        requestDispatcher->addHandler(backendHandler);
    }
}

//! Main function of the execution graph server backend.
int main(int argc, const char* argv[])
{
    // Parse command line arguments.
    ServerCLArgs args(argc, argv);

    // Make all loggers.
    EXECGRAPH_INSTANCIATE_SINGLETON_CTOR(Loggers, loggers, (args.logPath()));

    // Make a allocator for messages (requests/responses etc.)
    auto allocator = std::make_shared<BufferPool>();

    // Make the backend request dispatcher.
    auto dispatcher = std::make_shared<BackendRequestDispatcher>();
    setupBackends(dispatcher);

    EXECGRAPHGUI_BACKENDLOG_INFO(
        "Starting ExecutionGraph Server at '{0}:{1} with '{2}' threads.\n"
        "Logs located at '{3}'\n"
        "Root path: '{4}'",
        args.address(),
        args.port(),
        args.threads(),
        args.logPath(),
        args.rootPath());

#if 1
    // The io_context is required for all I/O.
    const auto threads = args.threads();
    boost::asio::io_context ioc{static_cast<int>(threads)};

    // Create and launch a listening port.
    const auto address = boost::asio::ip::make_address(args.address());

    auto listener = std::make_shared<HttpListener<HttpSession>>(
        ioc,
        boost::asio::ip::tcp::endpoint{address, args.port()},
        HttpSession::Factory{args.rootPath(), dispatcher, allocator});
    listener->run();

    auto ths = runWorkers(ioc, threads);

    // If we get here, it means we got a SIGINT or SIGTERM or an E
    // Block until all the threads exit
    for(auto& th : ths)
    {
        th.join();
    }
#else
    using tcp = boost::asio::ip::tcp;

    // The io_context is required for all I/O
    const auto threads = args->threads();
    boost::asio::io_context ioc{1};

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
    tcp::acceptor acceptor{ioc, {address, args->port()}};

    std::list<HttpWorker> workers;
    for(auto i = 0u; i < threads; ++i)
    {
        workers.emplace_back(acceptor, args->rootPath(), fmt::format("Worker {0}", i));
        workers.back().start();
    }
    ioc.run();

#endif

    EXECGRAPHGUI_BACKENDLOG_INFO("ExecutionGraph Server shutdown.");
    return EXIT_SUCCESS;
}