// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Dec 19 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGui_server_BinaryBufferBody_hpp
#define executionGraphGui_server_BinaryBufferBody_hpp

#include <boost/asio/buffer.hpp>
#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/core/detail/type_traits.hpp>
#include <boost/beast/http/error.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/optional.hpp>
#include <executionGraphGui/common/BinaryPayload.hpp>
#include <executionGraphGui/common/Loggers.hpp>

//! A @b Body using a `BinaryBuffer`
//!
//!  This body uses `BinaryBuffer` as a memory-based container
//!  for holding message payloads. Messages using this body type
//!  may be serialized and parsed.
struct BinaryBufferBody
{
    template<bool isRequest, class Fields>
    using header     = boost::beast::http::header<isRequest, Fields>;
    using error_code = boost::beast::error_code;
    using error      = boost::beast::http::error;

public:
    //! The type of container used for the body
    //! This determines the type of @ref message::body
    //! when this body type is used with a message container.
    //!
    using value_type = BinaryPayload::Buffer;

    //! Returns the payload size of the body
    //! When this body is used with @ref message::prepare_payload,
    //! the Content-Length will be set to the payload size, and
    //! any chunked Transfer-Encoding will be removed.
    static std::uint64_t size(const value_type& body)
    {
        return body.size();
    }

    //! The algorithm for parsing the body
    //! Meets the requirements of @b BodyReader.
    class reader
    {
        value_type& m_body;

    public:
        template<bool isRequest, class Fields>
        explicit reader(header<isRequest, Fields>&, value_type& b)
            : m_body(b)
        {
        }

        void init(const boost::optional<std::uint64_t>& length, error_code& ec)
        {
            if(length)
            {
                try
                {
                    m_body.reserve(*length);
                }
                catch(std::exception const&)
                {
                    ec = error::buffer_overflow;
                    return;
                }
            }
            ec.assign(0, ec.category());
        }

        template<class ConstBufferSequence>
        std::size_t put(const ConstBufferSequence& buffers, error_code& ec)
        {
            using boost::asio::buffer_copy;
            using boost::asio::buffer_size;
            auto const n   = buffer_size(buffers);
            auto const len = m_body.size();
            try
            {
                m_body.resize(len + n);
            }
            catch(std::exception const&)
            {
                ec = error::buffer_overflow;
                return 0;
            }
            ec.assign(0, ec.category());
            return buffer_copy(boost::asio::buffer(m_body.data() + len, n), buffers);
        }

        void
        finish(error_code& ec)
        {
            ec.assign(0, ec.category());
        }
    };

    //! The algorithm for serializing the body
    //! Meets the requirements of @b BodyWriter.
    class writer
    {
        const value_type& m_body;

    public:
        using const_buffers_type =
            boost::asio::const_buffer;

        template<bool isRequest, class Fields>
        explicit writer(const header<isRequest, Fields>&, const value_type& b)
            : m_body(b)
        {}

        void init(error_code& ec)
        {
            ec.assign(0, ec.category());
        }

        boost::optional<std::pair<const_buffers_type, bool>>
        get(error_code& ec)
        {
            ec.assign(0, ec.category());
            return {{const_buffers_type{m_body.data(), m_body.size()}, false}};
        }
    };
};

#endif
