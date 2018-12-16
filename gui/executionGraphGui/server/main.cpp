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
#include "executionGraphGui/server/HttpCommon.hpp"
#include "executionGraphGui/server/HttpListener.hpp"
#include "executionGraphGui/server/ServerCLArgs.hpp"

//! Main function of the execution graph server backend.
int main(int argc, const char* argv[])
{
    using namespace executionGraphGui;

    // Parse command line arguments
    EXECGRAPH_INSTANCIATE_SINGLETON_CTOR(ServerCLArgs, args, (argc, argv));

    // The io_context is required for all I/O
    const auto threads = args->threads();
    boost::asio::io_context ioc{static_cast<int>(threads)};

    // Create and launch a listening port
    const auto address = boost::asio::ip::make_address(args->address());
    std::make_shared<executionGraphGui::HttpListener>(
        ioc,
        boost::asio::ip::tcp::endpoint{address, args->port()},
        args->rootPath())
        ->run();

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
        v.emplace_back(
            [&ioc] {
                ioc.run();
            });
    ioc.run();

    return EXIT_SUCCESS;
}