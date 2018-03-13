// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Mon Mar 05 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef cefapp_BinaryPayload
#define cefapp_BinaryPayload

#include <string>
#include "common/BinaryBuffer.hpp"
#include "common/BufferPool.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    A movable-only wrapper around a binary buffer describing the payload.

    @date Mon Mar 05 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class BinaryPayload final
{
public:
    using Buffer = BinaryBuffer<BufferPool>;

public:
    //! Representing an empty payload!
    template<typename TBuffer>
    BinaryPayload(TBuffer&& buffer)
        : m_buffer(std::forward<TBuffer>(buffer)){};

    template<typename TBuffer>
    BinaryPayload(TBuffer&& buffer, const std::string& mimeType)
        : m_buffer(std::forward<TBuffer>(buffer)), m_mimeType(mimeType)
    {
    }

    void setMIMEType(const std::string& mimeType) { m_mimeType = mimeType; }

    //! No copy
    BinaryPayload(const BinaryPayload&) = delete;
    BinaryPayload& operator=(const BinaryPayload&) = delete;

    //! Move allowed
    BinaryPayload(BinaryPayload&&) = default;
    BinaryPayload& operator=(BinaryPayload&&) = default;

    Buffer& getBuffer() { return m_buffer; }
    const Buffer& getBuffer() const { return m_buffer; }

    const std::string& getMIMEType() const { return m_mimeType; }

private:
    Buffer m_buffer;         //!< The binary buffer.
    std::string m_mimeType;  //!< The MIME type of the binary buffer.
};

#endif
