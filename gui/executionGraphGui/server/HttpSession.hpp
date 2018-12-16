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
#include "executionGraph/common/FileSystem.hpp"
#include "executionGraphGui/server/HttpCommon.hpp"

// Handles an HTTP server connection
class HttpSession : public std::enable_shared_from_this<HttpSession>
{
public:
    using Body    = boost::beast::http::string_body;
    using Request = boost::beast::http::request<Body>;

    using ResponseString = boost::beast::http::response<Body>;
    using ResponseFile   = boost::beast::http::response<boost::beast::http::file_body>;
    using ResponseEmpty  = boost::beast::http::response<boost::beast::http::empty_body>;

private:
    using tcp = boost::asio::ip::tcp;
    struct Send;  //!< Functor to send the response.

private:
    tcp::socket m_socket;                                                  //!< The socket this session is running on.
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;  //!< The executor strand.

    boost::beast::flat_buffer m_buffer;  //!< A flat buffer where the request is stored.
    Request m_request;                   //!< The incoming request we are handling.
    std::shared_ptr<void> m_response;    //!< Response we are sending back.

    const std::path m_rootPath;  //!< Root file path for the server.

public:
    explicit HttpSession(tcp::socket socket,
                         const std::path& rootPath)
        : m_socket(std::move(socket))
        , m_strand(m_socket.get_executor())
        , m_rootPath(rootPath)
    {}

    void run();

    void doRead();

    void onRead(boost::system::error_code ec,
                std::size_t bytes_transferred);

    void onWrite(boost::system::error_code ec,
                 std::size_t bytes_transferred,
                 bool close);

    void doClose();
};
#endif