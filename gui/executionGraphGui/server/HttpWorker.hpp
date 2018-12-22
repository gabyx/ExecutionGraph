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

#ifndef executionGraphGui_server_HttpWorker_hpp
#define executionGraphGui_server_HttpWorker_hpp

#include <memory>
#include <optional>
#include <string>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include "executionGraph/common/FileSystem.hpp"
#include "executionGraphGui/server/HttpFieldAllocator.hpp"

class HttpWorker
{
private:
    using Tcp      = boost::asio::ip::tcp;
    using Acceptor = boost::asio::ip::tcp::acceptor;
    using Socket   = boost::asio::ip::tcp::socket;

    using FieldAllocator = std::allocator<char>;
    using BasicFields    = boost::beast::http::basic_fields<FieldAllocator>;

    //using RequestBody = boost::beast::http::basic_dynamic_body<beast::flat_static_buffer<1024 * 1024>>;
    using RequestStringBody = boost::beast::http::string_body;

    using ResponseStringBody = RequestStringBody;
    using ResponseFileBody   = boost::beast::http::file_body;

    using Request       = boost::beast::http::request<RequestStringBody, BasicFields>;
    using RequestParser = boost::beast::http::request_parser<RequestStringBody, FieldAllocator>;

    using ResponseString           = boost::beast::http::response<RequestStringBody, BasicFields>;
    using ResponseStringSerializer = boost::beast::http::response_serializer<RequestStringBody, BasicFields>;

    using ResponseFile           = boost::beast::http::response<ResponseFileBody, BasicFields>;
    using ResponseFileSerializer = boost::beast::http::response_serializer<ResponseFileBody, BasicFields>;

public:
    HttpWorker(Acceptor& acceptor,
               const std::path& rootPath,
               const std::string& name = "HttpWorker")
        : m_acceptor(acceptor)
        , m_rootPath(rootPath)
        , m_name(name)
    {}

    HttpWorker(const HttpWorker&) = delete;
    HttpWorker& operator=(const HttpWorker&) = delete;

    void start();

private:
    void accept();
    void readRequest();
    void processRequest(const Request& request);

    void sendBadResponse(boost::beast::http::status status,
                         const std::string& error);

    void sendFile(std::string_view target);

    void checkDeadline();

private:
    Acceptor& m_acceptor;                                  //!< The acceptor used to listen for incoming connections.
    Socket m_socket{m_acceptor.get_executor().context()};  //!< The socket for the currently connected client.

    boost::beast::flat_buffer m_buffer;  //!< The linear dynamic buffer for performing reads.
    FieldAllocator m_alloc;              //!< The allocator used for the fields in the request and reply.

    std::optional<RequestParser> m_parser;  //!< The parser for reading the requests.

    std::optional<ResponseString> m_responseString;                      //!< The string-based response message.
    std::optional<ResponseStringSerializer> m_responseStringSerializer;  //!< The string-based response serializer.

    std::optional<ResponseFile> m_responseFile;                      //!< The file-based response message.
    std::optional<ResponseFileSerializer> m_responseFileSerializer;  //!< The file-based response serializer.

    boost::asio::basic_waitable_timer<std::chrono::steady_clock> m_requestDeadline{
        m_acceptor.get_executor().context(),
        (std::chrono::steady_clock::time_point::max)()};  //!< The timer putting a time limit on requests.

    const std::path m_rootPath;  //!< The path to the root of the document directory.
    const std::string m_name;    //! The name of this worker.
};

#endif