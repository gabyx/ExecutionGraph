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
#include "executionGraphGui/common/RequestError.hpp"
#include "executionGraphGui/server/BackendRequestDispatcher.hpp"
#include "executionGraphGui/server/MimeType.hpp"

namespace http = boost::beast::http;  // from <boost/beast/http.hpp>

namespace
{
    using RequestBinary  = HttpSession::RequestBinary;
    using ResponseBinary = HttpSession::ResponseBinary;
    using ResponseString = HttpSession::ResponseString;
    using ResponseEmpty  = HttpSession::ResponseEmpty;
    using ResponseFile   = HttpSession::ResponseFile;

    static const auto versionString = getServerVersion();

    //! Returns a bad request response.
    template<typename Request, typename T>
    auto makeBadResponse(const Request& req, T&& why)
    {
        ResponseString res{http::status::bad_request, req.version()};
        res.set(http::field::server, versionString);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::forward<decltype(why)>(why);
        res.prepare_payload();
        return res;
    };

    //! Returns a not found response.
    template<typename Request, typename T>
    auto makeNotFound(const Request& req, T&& path)
    {
        ResponseString res{http::status::not_found, req.version()};
        res.set(http::field::server, versionString);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = fmt::format("The requested resource '{0}' was not found.", path);
        res.prepare_payload();
        return res;
    };

    //! Returns a server error response.
    template<typename Request, typename T>
    auto makeServerError(const Request& req, T&& what)
    {
        ResponseString res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, versionString);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = fmt::format("An error occurred: '{0}'", what);
        res.prepare_payload();
        return res;
    }

    template<typename Request>
    bool isTargetInvalid(Request&& req)
    {
        // Request path must be absolute and not contain "..".
        return req.target().empty() ||
               req.target()[0] != '/' ||
               req.target().find("..") != std::string_view::npos;
    }

    template<typename Request, typename Send>
    void handleRequestFileFallback(const std::path& rootPath,
                                   Request&& req,
                                   Send&& send)
    {
        // Make sure we can handle the method.
        if(req.method() != http::verb::get)
        {
            send(makeBadResponse(req, "Unknown HTTP-method."));
            return;
        }

        if(isTargetInvalid(req))
        {
            send(makeBadResponse(req, "Illegal request-target."));
            return;
        }

        // Build the path to the requested file.
        std::path path = rootPath / req.target().substr(1);
        if(!std::filesystem::is_regular_file(path))
        {
            path = rootPath / "index.html";
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
            send(makeNotFound(req, req.target()));
            return;
        }

        // Handle an unknown error.
        if(ec)
        {
            send(makeServerError(req, ec.message()));
            return;
        }

        // Cache the size since we need it after the move.
        auto const size = body.size();

        // Respond to GET request.
        ResponseFile res{std::piecewise_construct,
                         std::make_tuple(std::move(body)),
                         std::make_tuple(http::status::ok, req.version())};
        res.set(http::field::server, versionString);
        res.set(http::field::content_type, getMimeType(path));
        res.content_length(size);
        res.prepare_payload();
        res.keep_alive(req.keep_alive());
        send(std::move(res));
        return;
    }

    //! @brief Handle the request.
    template<typename Request,
             typename Send,
             typename Dispatcher,
             typename Executor,
             typename Allocator>
    void handleRequestBackend(const std::path& rootPath,
                              Request&& req,
                              std::uint64_t payLoadSize,
                              Dispatcher&& dispatcher,
                              Send&& send,
                              Executor& executor,
                              Allocator& allocator)
    {
        if(isTargetInvalid(req))
        {
            send(makeBadResponse(req, "Illegal request-target."));
            return;
        }

        BackendRequest request(req.target(),
                               payLoadSize ? std::make_optional(
                                                 BackendRequest::Payload{
                                                     std::move(req.body()),
                                                     req[http::field::content_type]})
                                           : std::nullopt);

        BackendResponsePromise responsePromise{executionGraph::Id{}, allocator};
        ResponseFuture responeFuture(responsePromise);

        if(!dispatcher.handleRequest(request, responsePromise))
        {
            send(makeBadResponse(req, "Request is not handled in dispatcher!"));
        }
        else
        {
            try
            {
                EXECGRAPHGUI_THROW_IF(!responeFuture.isValid(), "Future is not valid!");
                auto payload  = responeFuture.waitForPayload();
                auto mimeType = payload.mimeType();

                // Send the response.
                ResponseBinary res{std::piecewise_construct,
                                   std::make_tuple(std::move(payload.buffer())),
                                   std::make_tuple(http::status::ok, req.version())};
                res.set(http::field::server, versionString);
                res.set(http::field::content_type, mimeType);
                res.content_length(res.payload_size());
                res.keep_alive(req.keep_alive());
                res.prepare_payload();
                send(std::move(res));
            }
            catch(const BadRequestError& e)
            {
                send(makeBadResponse(req, fmt::format("BadRequest: '{0}'", e.what())));
            }
            catch(const InternalBackendError& e)
            {
                send(makeServerError(req, fmt::format("InternalBackendError: '{0}'", e.what())));
            }
            catch(const std::exception& e)
            {
                send(makeServerError(req, fmt::format("Unknown Error: '{0}'", e.what())));
            }
        }
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
    Send(std::shared_ptr<HttpSession> session)
        : m_session(session) {}

    std::shared_ptr<HttpSession> m_session;

    template<typename Message>
    void operator()(Message&& msg)
    {
        EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpSession @{0}:: post send response ...",
                                      m_session);

        // The lifetime of the message has to extend
        // for the duration of the async operation so
        // we use a shared_ptr to manage it.
        auto m = std::make_shared<Message>(std::move(msg));

        auto onCompletion = [session = m_session, m](auto ec, std::size_t bytesTransferred) {
            session->onWrite(ec, bytesTransferred, m->need_eof());
        };

        // Write the response.
        http::async_write(m_session->m_socket,
                          *m,
                          boost::asio::bind_executor(m_session->m_strand, onCompletion));
    }
};

HttpSession::HttpSession(tcp::socket socket,
                         const std::path& rootPath,
                         std::shared_ptr<BackendRequestDispatcher> dispatcher,
                         std::shared_ptr<BufferPool> allocator)
    : m_socket(std::move(socket))
    , m_strand(m_socket.get_executor())
    , m_request({}, allocator)
    , m_rootPath(rootPath)
    , m_dispatcher(dispatcher)
    , m_allocator(allocator)
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
    m_request.base() = {};
    m_request.body().resize(0);

    auto onCompletion = [session = shared_from_this()](auto ec, std::size_t bytesTransferred) {
        session->onRead(ec, bytesTransferred);
    };

    // Read a request.
    EXECGRAPHGUI_BACKENDLOG_DEBUG("HttpSession @{0}:: post read request ...", fmt::ptr(this));
    http::async_read(m_socket,
                     m_buffer,
                     m_request,
                     boost::asio::bind_executor(m_strand,
                                                onCompletion));
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

    auto payloadSize = m_request.payload_size();

    EXECGRAPHGUI_BACKENDLOG_DEBUG(
        "HttpSession @{0} ::handleRequest : "
        "Request[ method: '{1}' target: '{2}', payload: '{3}' bytes ]",
        fmt::ptr(this),
        m_request.method(),
        m_request.target(),
        payloadSize ? *payloadSize : 0);

    //handleRequestFileFallback(m_rootPath, std::move(m_request), Send{shared_from_this()});

    // Send the response.
    handleRequestBackend(m_rootPath,
                         std::move(m_request),
                         payloadSize ? *payloadSize : 0,
                         *m_dispatcher,
                         Send{shared_from_this()},
                         m_strand,
                         m_allocator);
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
