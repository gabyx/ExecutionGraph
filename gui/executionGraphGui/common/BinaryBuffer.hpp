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

#ifndef common_BinaryBuffer_hpp
#define common_BinaryBuffer_hpp

#include <cstdint>
#include <foonathan/memory/smart_ptr.hpp>
#include <memory>

/* ---------------------------------------------------------------------------------------*/
/*!
    Simple Binary Buffer
    Can be moved around but not copied.

    @date Mon Mar 05 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
template<typename RawAllocator>
class BinaryBuffer final
{
private:
    using Deleter = foonathan::memory::allocator_deleter<uint8_t[], RawAllocator>;

public:
    using BufferPtr      = std::unique_ptr<uint8_t[], Deleter>;
    using iterator       = uint8_t*;
    using const_iterator = const uint8_t*;

public:
    //! Constructor for an empty buffer!
    BinaryBuffer() = default;

    //! Constructor for a meaningful buffer!
    BinaryBuffer(std::size_t bytes, std::shared_ptr<RawAllocator> allocator)
        : m_allocator(allocator)
        , m_data(foonathan::memory::allocate_unique<uint8_t[]>(*allocator, bytes))
        , m_bytes(bytes)
    {
    }

    //! Copy prohibited
    BinaryBuffer(const BinaryBuffer&) = delete;
    BinaryBuffer& operator=(const BinaryBuffer&) = delete;

    //! Move allowed
    BinaryBuffer(BinaryBuffer&&) = default;
    BinaryBuffer& operator=(BinaryBuffer&&) = default;

    //! Begin/End Iterators
    //@{
    iterator begin() { return getData(); }
    const_iterator begin() const { return getData(); }
    const_iterator cbegin() const { return begin(); }
    iterator end() { return getData() + getSize(); }
    const_iterator end() const { return getData() + getSize(); }
    const_iterator cend() const { return end(); }
    //@}

    //! Get the buffer pointer.
    uint8_t* getData() { return m_data.get(); }
    //! Get the constant buffer pointer.
    const uint8_t* getData() const { return m_data.get(); }

    //! Get the size in bytes of the buffer.
    std::size_t getSize() const { return m_bytes; }

    //! Check if buffer is empty (nullptr or no bytes)
    bool isEmpty() const { return getSize() == 0 || getData() == nullptr; }

private:
    std::shared_ptr<RawAllocator> m_allocator;  //!< Reference to the allocator.

    /*! Data pointer. It is guaranteed by this declaration order 
        that `m_data` is destroyed first, and then possibly the `m_allocator`! */
    BufferPtr m_data;

    std::size_t m_bytes = 0;  //!< Number of bytes.
};

#endif
