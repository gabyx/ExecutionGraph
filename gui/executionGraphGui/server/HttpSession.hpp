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
#ifndef executionGraphGui_server_HttpSession_hpp
#define executionGraphGui_server_HttpSession_hpp

#include <memory>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include "executionGraphGui/server/HttpCommon.hpp"

namespace executionGraphGui
{
    // Handles an HTTP server connection
    class HttpSession : public std::enable_shared_from_this<HttpSession>
    {
        template<typename Body>
        using request     = boost::beast::http::request<Body>;
        using string_body = boost::beast::http::string_body;
        using tcp         = boost::asio::ip::tcp;

        tcp::socket m_socket;
        boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
        boost::beast::flat_buffer m_buffer;
        const std::string m_doc_root;
        request<string_body> m_req;
        std::shared_ptr<void> m_res;

        struct Send;

    public:
        explicit HttpSession(tcp::socket socket,
                             const std::string& doc_root)
            : m_socket(std::move(socket))
            , m_strand(m_socket.get_executor())
            , m_doc_root(doc_root)
        {}

        void run();

        void do_read();

        void on_read(boost::system::error_code ec,
                     std::size_t bytes_transferred);

        void on_write(boost::system::error_code ec,
                      std::size_t bytes_transferred,
                      bool close);

        void do_close();
    };

}  // namespace executionGraphGui

#endif