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

#include "executionGraphGui/server/HttpListener.hpp"
#include "executionGraphGui/server/HttpSession.hpp"

template<typename HttpSessionFactory>
HttpListener<HttpSessionFactory>::HttpListener(boost::asio::io_context& ioc,
                                               tcp::endpoint endpoint,
                                               HttpSessionFactory factory)
    : m_acceptor(ioc)
    , m_socket(ioc)
    , m_httpSessionFactory(std::move(factory))
{
    boost::system::error_code ec;

    // Open the acceptor
    m_acceptor.open(endpoint.protocol(), ec);
    if(ec)
    {
        fail(ec, "HttpListener:: open");
        return;
    }

    // Allow address reuse
    m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
    if(ec)
    {
        fail(ec, "HttpListener:: set_option");
        return;
    }

    // Bind to the server address
    m_acceptor.bind(endpoint, ec);
    if(ec)
    {
        fail(ec, "HttpListener:: bind");
        return;
    }

    // Start listening for connections
    m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
    if(ec)
    {
        fail(ec, "HttpListener:: listen");
        return;
    }
}

// Start accepting incoming connections
template<typename HttpSessionFactory>
void HttpListener<HttpSessionFactory>::run()
{
    if(!m_acceptor.is_open())
    {
        return;
    }
    doAccept();
}

template<typename HttpSessionFactory>
void HttpListener<HttpSessionFactory>::doAccept()
{
    m_acceptor.async_accept(
        m_socket,
        std::bind(
            &HttpListener::onAccept,
            shared_from_this(),
            std::placeholders::_1));
}

template<typename HttpSessionFactory>
void HttpListener<HttpSessionFactory>::onAccept(boost::system::error_code ec)
{
    if(ec)
    {
        fail(ec, "HttpListener:: accept");
    }
    else
    {
        // Create the session and run it.
        auto session = m_httpSessionFactory.create(std::move(m_socket));
        session->run();
    }

    // Accept another connection
    doAccept();
}
