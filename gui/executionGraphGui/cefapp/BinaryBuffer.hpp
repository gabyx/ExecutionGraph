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

#ifndef cefapp_BinaryBuffer_hpp
#define cefapp_BinaryBuffer_hpp

#include <cstdint>
#include <foonathan/memory/smart_ptr.hpp>
#include <memory>

/* ---------------------------------------------------------------------------------------*/
/*!
    Simple Binary Buffer View
    Can be moved around but not copied.

    @date Mon Mar 05 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
template<typename RawAllocator>
class BinaryBuffer
{
private:
    using AllocatorDeallocator = foonathan::memory::allocator_deallocator<uint8_t[], RawAllocator>;

public:
    using BufferPtr = std::unique_ptr<uint8_t[], AllocatorDeallocator>;

public:
    BinaryBuffer(std::size_t bytes, std::shared_ptr<RawAllocator> allocator)
        : m_data(foonathan::memory::allocate_unique<uint8_t[]>(*allocator, bytes))
        , m_bytes(bytes)
    {
    }

    BinaryBuffer(const BinaryBuffer&) = delete;
    BinaryBuffer& operator=(const BinaryBuffer&) = delete;

    BinaryBuffer(BinaryBuffer&&) = default;
    BinaryBuffer& operator=(BinaryBuffer&&) = default;

    uint8_t* getData() { return m_data.get(); }
    const uint8_t* getData() const { return m_data.get(); }

    std::size_t getBytes() { return m_bytes; }

private:
    BufferPtr m_data;                           //!< Data pointer.
    std::size_t m_bytes = 0;                    //!< Number of bytes.
    std::shared_ptr<RawAllocator> m_allocator;  //!< Reference to the allocator.
};

#endif