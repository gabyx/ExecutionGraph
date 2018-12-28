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

#ifndef executionGraphGui_common_BinaryPayload_hpp
#define executionGraphGui_common_BinaryPayload_hpp

#include <string_view>
#include "executionGraphGui/common/BinaryBuffer.hpp"
#include "executionGraphGui/common/BufferPool.hpp"

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
    //! Representing a meaningful payload.
    template<typename TBuffer>
    BinaryPayload(TBuffer&& buffer, const std::string_view& mimeType)
        : m_buffer(std::forward<TBuffer>(buffer)), m_mimeType(mimeType)
    {
    }
    
    //! No copy
    BinaryPayload(const BinaryPayload&) = delete;
    BinaryPayload& operator=(const BinaryPayload&) = delete;

    //! Move allowed
    BinaryPayload(BinaryPayload&&) = default;
    BinaryPayload& operator=(BinaryPayload&&) = default;

    Buffer& buffer() { return m_buffer; }
    const Buffer& buffer() const { return m_buffer; }

    const std::string& mimeType() const { return m_mimeType; }

private:
    Buffer m_buffer;         //!< The binary buffer.
    std::string m_mimeType;  //!< The MIME type of the binary buffer.
};

#endif
