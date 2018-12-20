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
#include "executionGraphGui/common/BufferPool.hpp"
#include "executionGraphGui/server/BinaryBufferBody.hpp"
#include "executionGraphGui/server/HttpCommon.hpp"

class BackendRequestDispatcher;

// Handles an HTTP server connection
class HttpSession : public std::enable_shared_from_this<HttpSession>
{
public:
    //! A simple factory creating this HTTP Sessions.
    struct Factory
    {
        Factory(const std::path& rootPath,
                std::shared_ptr<BackendRequestDispatcher> dispatcher,
                std::shared_ptr<BufferPool> allocator)
            : m_rootPath(rootPath)
            , m_dispatcher(dispatcher)
            , m_allocator(allocator)
        {}

        template<typename... Args>
        auto operator()(Args&&... args)
        {
            return std::make_shared<HttpSession>(std::forward<Args>(args)...,
                                                 m_rootPath,
                                                 m_dispatcher,
                                                 m_allocator);
        }

    private:
        const std::path& m_rootPath;
        std::shared_ptr<BackendRequestDispatcher> m_dispatcher;
        std::shared_ptr<BufferPool> m_allocator;
    };

public:
    using RequestBinary  = boost::beast::http::request<BinaryBufferBody>;
    using ResponseBinary = boost::beast::http::response<BinaryBufferBody>;
    using ResponseString = boost::beast::http::response<boost::beast::http::string_body>;
    using ResponseFile   = boost::beast::http::response<boost::beast::http::file_body>;
    using ResponseEmpty  = boost::beast::http::response<boost::beast::http::empty_body>;

private:
    using tcp = boost::asio::ip::tcp;

    struct Send;

private:
    tcp::socket m_socket;  //!< The socket this session is running on.

    boost::asio::strand<
        boost::asio::io_context::executor_type>
        m_strand;  //!< The executor strand (serialized completion handler dispatch).

    boost::beast::flat_buffer m_buffer;  //!< A linear continuous buffer where the request is stored.
    RequestBinary m_request;             //!< The incoming request we are handling.

    const std::path& m_rootPath;
    std::shared_ptr<BackendRequestDispatcher> m_dispatcher;  //!< The backend request dispatcher.
    std::shared_ptr<BufferPool> m_allocator;                 //!< Buffer allocator.

public:
    explicit HttpSession(tcp::socket socket,
                         const std::path& rootPath,
                         std::shared_ptr<BackendRequestDispatcher> dispatcher,
                         std::shared_ptr<BufferPool> allocator);

    ~HttpSession();

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