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

#include "executionGraphGui/server/HttpSession.hpp"
#include <functional>
#include <tuple>
#include <boost/asio/bind_executor.hpp>
#include "executionGraphGui/server/MimeType.hpp"

namespace http = boost::beast::http;  // from <boost/beast/http.hpp>

namespace
{
    using Body    = HttpSession::Body;
    using Request = HttpSession::Request;

    using ResponseString = HttpSession::ResponseString;
    using ResponseEmpty  = HttpSession::ResponseEmpty;
    using ResponseFile   = HttpSession::ResponseFile;

    //! @brief Handle the request.
    //! This function produces an HTTP response for the given
    //! Request. The type of the response object depends on the
    //! contents of the Request, so the interface requires the
    //! caller to pass a generic lambda for receiving the response.
    template<class Request, class Send>
    void handleRequest(const std::path& rootPath,
                       Request&& req,
                       Send&& send)
    {
        EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpSession::Send::handleRequest(...)");
        static const auto versionString = getServerVersion();

        // Returns a bad Request response.
        auto const badRequest =
            [&req](auto&& why) {
                ResponseString res{http::status::bad_request, req.version()};
                res.set(http::field::server, versionString);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = std::forward<decltype(why)>(why);
                res.prepare_payload();
                return res;
            };

        // Returns a not found response.
        auto const notFound =
            [&req](auto&& path) {
                ResponseString res{http::status::not_found, req.version()};
                res.set(http::field::server, versionString);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = fmt::format("The requested resource '{0}' was not found.", path);
                res.prepare_payload();
                return res;
            };

        // Returns a server error response.
        auto const serverError =
            [&req](auto&& what) {
                ResponseString res{http::status::internal_server_error, req.version()};
                res.set(http::field::server, versionString);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = fmt::format("An error occurred: '{0}'", what);
                res.prepare_payload();
                return res;
            };

        // Make sure we can handle the method.
        if(req.method() != http::verb::get &&
           req.method() != http::verb::head)

        {
            return send(badRequest("Unknown HTTP-method."));
        }

        // Request path must be absolute and not contain "..".
        if(req.target().empty() ||
           req.target()[0] != '/' ||
           req.target().find("..") != boost::beast::string_view::npos)
        {
            return send(badRequest("Illegal request-target."));
        }

        // Build the path to the requested file.
        std::path path = (rootPath / (std::string{"."} + std::string{req.target()}))
                             .lexically_normal();

        if(req.target().back() == '/')
        {
            path.append("index.html");
        }

        // Attempt to open the file.
        boost::beast::error_code ec;
        http::file_body::value_type body;
        body.open(path.string().c_str(),
                  boost::beast::file_mode::scan,
                  ec);

        // Handle the case where the file doesn't exist.
        if(ec == boost::system::errc::no_such_file_or_directory)
        {
            return send(notFound(req.target()));
        }

        // Handle an unknown error.
        if(ec)
        {
            return send(serverError(ec.message()));
        }

        // Cache the size since we need it after the move.
        auto const size = body.size();

        // Respond to HEAD request.
        if(req.method() == http::verb::head)
        {
            ResponseEmpty res{http::status::ok, req.version()};
            res.set(http::field::server, versionString);
            res.set(http::field::content_type, getMimeType(path));
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return send(std::move(res));
        }

        // Respond to GET request.
        ResponseFile res{std::piecewise_construct,
                         std::make_tuple(std::move(body)),
                         std::make_tuple(http::status::ok, req.version())};
        res.set(http::field::server, versionString);
        res.set(http::field::content_type, getMimeType(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
    }

}  // namespace

/* ---------------------------------------------------------------------------------------*/
/*!
    Send functor which sends the response.

    @date Fri Dec 14 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
*/
/* ---------------------------------------------------------------------------------------*/
struct HttpSession::Send
{
    HttpSession& m_self;

    explicit Send(HttpSession& self)
        : m_self(self)
    {}

    template<typename Message>
    void operator()(Message&& msg) const;
};

HttpSession::HttpSession(tcp::socket socket,
                         const std::path& rootPath)
    : m_socket(std::move(socket))
    , m_strand(m_socket.get_executor())
    , m_rootPath(rootPath)
{
    EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpSession @{0}:: Ctor", fmt::ptr(this));
}

HttpSession::~HttpSession()
{
    EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpSession @{0}:: Dtor", fmt::ptr(this));
}

//! Start the asynchronous operation.
void HttpSession::run()
{
    doRead();
}

void HttpSession::doRead()
{
    // Make the request empty before reading,
    // otherwise the operation behavior is undefined.
    m_request = {};

    // Read a request.
    EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpSession @{0}:: async read request ...", fmt::ptr(this));
    http::async_read(m_socket,
                     m_buffer,
                     m_request,
                     boost::asio::bind_executor(m_strand,
                                                std::bind(&HttpSession::onRead,
                                                          shared_from_this(),
                                                          std::placeholders::_1,
                                                          std::placeholders::_2)));
}

void HttpSession::onRead(boost::system::error_code ec,
                         std::size_t bytesTransferred)
{
    boost::ignore_unused(bytesTransferred);

    // This means they closed the connection.
    if(ec == http::error::end_of_stream)
    {
        EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpSession @{0}::onRead : end of stream --> close()", fmt::ptr(this));
        return doClose();
    }

    if(ec)
    {
        return fail(ec, "HttpSession:: read");
    }

    EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpSession @{0} ::onRead : Request[ method: '{1}' target: '{2}']",
                                  fmt::ptr(this),
                                  m_request.method(),
                                  m_request.target());

    // Send the response.
    handleRequest(m_rootPath,
                  std::move(m_request),
                  Send{*this});
}

void HttpSession::onWrite(boost::system::error_code ec,
                          std::size_t bytesTransferred,
                          bool close)
{
    EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpSession @{0}::onWrite(...)", fmt::ptr(this));
    boost::ignore_unused(bytesTransferred);

    if(ec)
    {
        return fail(ec, "HttpSession:: write");
    }

    if(close)
    {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return doClose();
    }

    // We're done with the response so delete it.
    m_response = nullptr;

    // Read another request.
    doRead();
}

void HttpSession::doClose()
{
    EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpSession @{0}:: doClose()", fmt::ptr(this));
    // Send a TCP shutdown
    boost::system::error_code ec;
    m_socket.shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}

template<typename Message>
void HttpSession::Send::operator()(Message&& msg) const
{
    EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpSession @{0}:: async send response ...", fmt::ptr(&m_self));

    // The lifetime of the message has to extend
    // for the duration of the async operation so
    // we use a shared_ptr to manage it.
    auto sp = std::make_shared<Message>(std::move(msg));

    // Store a type-erased version of the shared
    // pointer in the class to keep it alive.
    //@todo why store in class, make lambda!
    m_self.m_response = sp;

    // Write the response
    http::async_write(
        m_self.m_socket,
        *sp,
        boost::asio::bind_executor(
            m_self.m_strand,
            std::bind(
                &HttpSession::onWrite,
                m_self.shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2,
                sp->need_eof())));
}
