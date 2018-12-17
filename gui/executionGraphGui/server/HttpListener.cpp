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

namespace http = boost::beast::http;  // from <boost/beast/http.hpp>

HttpListener::HttpListener(boost::asio::io_context& ioc,
                           tcp::endpoint endpoint,
                           const std::string& doc_root)
    : m_acceptor(ioc)
    , m_socket(ioc)
    , m_rootPath(doc_root)
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
    m_acceptor.listen(
        boost::asio::socket_base::max_listen_connections, ec);
    if(ec)
    {
        fail(ec, "HttpListener:: listen");
        return;
    }
}

// Start accepting incoming connections
void HttpListener::run()
{
    if(!m_acceptor.is_open())
    {
        return;
    }
    doAccept();
}

void HttpListener::doAccept()
{
    EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpListener:: async accept ...");
    m_acceptor.async_accept(
        m_socket,
        std::bind(
            &HttpListener::onAccept,
            shared_from_this(),
            std::placeholders::_1));
}

void HttpListener::onAccept(boost::system::error_code ec)
{
    if(ec)
    {
        fail(ec, "HttpListener:: accept");
    }
    else
    {
        EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpListener:: accept ...");
        // Create the session and run it
        std::make_shared<HttpSession>(
            std::move(m_socket),
            m_rootPath)
            ->run();
    }

    // Accept another connection
    doAccept();
}
