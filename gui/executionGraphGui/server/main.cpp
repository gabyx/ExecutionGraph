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
#include <string>
#include <thread>
#include <vector>
#include <boost/asio/ip/tcp.hpp>
#include "executionGraphGui/server/HttpListener.hpp"

using tcp      = boost::asio::ip::tcp;  // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

int main(int argc, char* argv[])
{
    // Check command line arguments.
    if(argc != 5)
    {
        std::cerr << "Usage: http-server-async <address> <port> <doc_root> <threads>\n"
                  << "Example:\n"
                  << "    http-server-async 0.0.0.0 8080 . 1\n";
        return EXIT_FAILURE;
    }
    auto const address  = boost::asio::ip::make_address(argv[1]);
    auto const port     = static_cast<unsigned short>(std::atoi(argv[2]));
    auto const doc_root = std::string(argv[3]);
    auto const threads  = std::max<int>(1, std::atoi(argv[4]));

    // The io_context is required for all I/O
    boost::asio::io_context ioc{threads};

    // Create and launch a listening port
    std::make_shared<executionGraphGui::HttpListener>(ioc,
                                                      tcp::endpoint{address, port},
                                                      doc_root)
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