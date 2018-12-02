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

namespace executionGraphGui
{
    // Handles an HTTP server connection
    class HttpSession : public std::enable_shared_from_this<HttpSession>
    {
        // This is the C++11 equivalent of a generic lambda.
        // The function object is used to send an HTTP message.
        struct send_lambda
        {
            HttpSession& m_self;

            explicit send_lambda(HttpSession& self)
                : m_self(self)
            {
            }

            template<bool isRequest, class Body, class Fields>
            void
            operator()(http::message<isRequest, Body, Fields>&& msg) const
            {
                // The lifetime of the message has to extend
                // for the duration of the async operation so
                // we use a shared_ptr to manage it.
                auto sp = std::make_shared<
                    http::message<isRequest, Body, Fields>>(std::move(msg));

                // Store a type-erased version of the shared
                // pointer in the class to keep it alive.
                m_self.m_res = sp;

                // Write the response
                http::async_write(
                    m_self.m_socket,
                    *sp,
                    boost::asio::bind_executor(
                        m_self.m_strand,
                        std::bind(
                            &HttpSession::on_write,
                            m_self.shared_from_this(),
                            std::placeholders::_1,
                            std::placeholders::_2,
                            sp->need_eof())));
            }
        };

        tcp::socket m_socket;
        boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
        boost::beast::flat_buffer m_buffer;
        const std::string m_doc_root;
        http::request<http::string_body> m_req;
        std::shared_ptr<void> m_res;
        send_lambda m_lambda;

    public:
        // Take ownership of the socket
        explicit HttpSession(
            tcp::socket socket,
            const std::string& doc_root)
            : m_socket(std::move(socket))
            , m_strand(m_socket.get_executor())
            , m_doc_root(doc_root)
            , m_lambda(*this)
        {
        }

        // Start the asynchronous operation
        void
        run()
        {
            do_read();
        }

        void
        do_read()
        {
            // Make the request empty before reading,
            // otherwise the operation behavior is undefined.
            m_req = {};

            // Read a request
            http::async_read(m_socket,
                             m_buffer,
                             m_req,
                             boost::asio::bind_executor(m_strand,
                                                        std::bind(&HttpSession::on_read,
                                                                  shared_from_this(),
                                                                  std::placeholders::_1,
                                                                  std::placeholders::_2)));
        }

        void
        on_read(
            boost::system::error_code ec,
            std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);

            // This means they closed the connection
            if(ec == http::error::end_of_stream)
            {
                return do_close();
            }
            if(ec)
            {
                return fail(ec, "read");
            }

            // Send the response
            handle_request(*m_doc_root, std::move(m_req), m_lambda);
        }

        void
        on_write(
            boost::system::error_code ec,
            std::size_t bytes_transferred,
            bool close)
        {
            boost::ignore_unused(bytes_transferred);

            if(ec)
            {
                return fail(ec, "write");
            }

            if(close)
            {
                // This means we should close the connection, usually because
                // the response indicated the "Connection: close" semantic.
                return do_close();
            }

            // We're done with the response so delete it
            m_res = nullptr;

            // Read another request
            do_read();
        }

        void
        do_close()
        {
            // Send a TCP shutdown
            boost::system::error_code ec;
            m_socket.shutdown(tcp::socket::shutdown_send, ec);

            // At this point the connection is closed gracefully
        }
    };

}  // namespace executionGraphGui