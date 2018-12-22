// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Dec 02 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================
#ifndef executionGraphGui_server_HttpListener_hpp
#define executionGraphGui_server_HttpListener_hpp

#include <memory>
#include <string>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/system/error_code.hpp>
#include "executionGraphGui/server/HttpCommon.hpp"

// Accepts incoming connections and launches the sessions
template<typename HttpSession>
class HttpListener : public std::enable_shared_from_this<HttpListener<HttpSession>>
{
    using tcp = boost::asio::ip::tcp;

public:
    using HttpSessionFactory = std::function<std::shared_ptr<HttpSession>(tcp::socket socket)>;

public:
    template<typename Factory>
    HttpListener(boost::asio::io_context& ioc,
                 tcp::endpoint endpoint,
                 Factory sessionFactory);

    void run();
    void doAccept();
    void onAccept(boost::system::error_code ec);

private:
    tcp::acceptor m_acceptor;  //!< Accpetor.
    tcp::socket m_socket;      //!< The socket we use to listen for incoming requests.

    HttpSessionFactory m_httpSessionFactory;  //!< Creating a HTTP Session.
};

#include "executionGraphGui/server/HttpListener.cpp"

#endif