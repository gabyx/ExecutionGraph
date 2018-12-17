// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Mon Dec 17 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#include "executionGraphGui/server/HttpWorker.hpp"
#include <chrono>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include "executionGraphGui/common/Loggers.hpp"
#include "executionGraphGui/server/MimeType.hpp"
#include "executionGraphGui/server/HttpCommon.hpp"

namespace beast = boost::beast;          // from <boost/beast.hpp>
namespace http  = beast::http;           // from <boost/beast/http.hpp>
namespace net   = boost::asio;           // from <boost/asio.hpp>
using tcp       = boost::asio::ip::tcp;  // from <boost/asio/ip/tcp.hpp>

void HttpWorker::start()
{
    accept();
    checkDeadline();
}

void HttpWorker::accept()
{
    // Clean up any previous connection.
    beast::error_code ec;
    m_socket.close(ec);
    m_buffer.consume(m_buffer.size());

    m_acceptor.async_accept(
        m_socket,
        [this](beast::error_code ec) {
            if(ec)
            {
                accept();
            }
            else
            {
                // Request must be fully processed within 60 seconds.
                m_requestDeadline.expires_after(std::chrono::seconds(60));
                readRequest();
            }
        });
}

void HttpWorker::readRequest()
{
    // On each read the parser needs to be destroyed and
    // recreated.
    // Arguments passed to the parser constructor are
    // forwarded to the message object. A single argument
    // is forwarded to the body constructor.
    //
    // We construct the dynamic body with a 1MB limit
    // to prevent vulnerability to buffer attacks.
    //
    m_parser.emplace(std::piecewise_construct,
                     std::make_tuple(),
                     std::make_tuple(m_alloc));

    http::async_read(
        m_socket,
        m_buffer,
        *m_parser,
        [this](beast::error_code ec, std::size_t) {
            if(ec)
            {
                accept();
            }
            else
            {
                processRequest(m_parser->get());
            }
        });
}

void HttpWorker::processRequest(const Request& request)
{
    switch(request.method())
    {
        case http::verb::get:
        {
            sendFile(request.target());
            break;
        }
        default:
        {  // We return responses indicating an error if
            // we do not recognize the request method.
            sendBadResponse(
                http::status::bad_request,
                fmt::format("Invalid request-method '{0}'", request.method()));
            break;
        }
    }
}

void HttpWorker::sendBadResponse(http::status status,
                                 std::string const& error)
{
    m_responseString.emplace(std::piecewise_construct,
                             std::make_tuple(),
                             std::make_tuple(m_alloc));

    m_responseString->result(status);
    m_responseString->keep_alive(false);
    m_responseString->set(http::field::server, "Beast");
    m_responseString->set(http::field::content_type, "text/plain");
    m_responseString->body() = error;
    m_responseString->prepare_payload();

    m_responseStringSerializer.emplace(*m_responseString);

    http::async_write(m_socket,
                      *m_responseStringSerializer,
                      [this](beast::error_code ec, std::size_t) {
                          m_socket.shutdown(tcp::socket::shutdown_send, ec);
                          m_responseStringSerializer.reset();
                          m_responseString.reset();
                          accept();
                      });
}

void HttpWorker::sendFile(beast::string_view target)
{
    // Request path must be absolute and not contain "..".
    if(target.empty() ||
       target[0] != '/' ||
       target.find("..") != std::string::npos)
    {
        sendBadResponse(
            http::status::not_found,
            fmt::format("Wrong request path: '{0}", target));
        return;
    }

    // Build the path to the requested file.
    std::path path = (m_rootPath / (std::string{"."} + target.to_string())).lexically_normal();

    if(target.back() == '/')
    {
        path.append("index.html");
    }

    http::file_body::value_type file;
    beast::error_code ec;
    file.open(path.c_str(),
              beast::file_mode::read,
              ec);
    if(ec)
    {
        sendBadResponse(http::status::not_found,
                        fmt::format("File path '{0}' not found", path));
        return;
    }

    m_responseFile.emplace(std::piecewise_construct,
                           std::make_tuple(),
                           std::make_tuple(m_alloc));

    m_responseFile->result(http::status::ok);
    m_responseFile->keep_alive(false);
    m_responseFile->set(http::field::server, "Beast");
    m_responseFile->set(http::field::content_type, getMimeType(path));
    m_responseFile->body() = std::move(file);
    m_responseFile->prepare_payload();

    m_responseFileSerializer.emplace(*m_responseFile);

    http::async_write(m_socket,
                      *m_responseFileSerializer,
                      [this](beast::error_code ec, std::size_t) {
                          m_socket.shutdown(tcp::socket::shutdown_send, ec);
                          m_responseFileSerializer.reset();
                          m_responseFile.reset();
                          accept();
                      });
}

void HttpWorker::checkDeadline()
{
    // The deadline may have moved, so check it has really passed.
    if(m_requestDeadline.expiry() <= std::chrono::steady_clock::now())
    {
        // Close socket to cancel any outstanding operation.
        m_socket.close();

        // Sleep indefinitely until we're given a new deadline.
        m_requestDeadline.expires_at(
            std::chrono::steady_clock::time_point::max());
    }

    m_requestDeadline.async_wait([this](beast::error_code) {
        checkDeadline();
    });
}