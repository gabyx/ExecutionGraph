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

#ifndef executionGraphGui_common_BinaryBuffer_hpp
#define executionGraphGui_common_BinaryBuffer_hpp

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

    //! Constructor for a meaningful buffer with size `bytes` allocated by `allocator`.
    BinaryBuffer(std::shared_ptr<RawAllocator> allocator,
                 std::size_t bytes)
        : m_allocator(allocator)
        , m_buffer(foonathan::memory::allocate_unique<uint8_t[]>(*allocator, bytes))
        , m_allocatedBytes(bytes)
        , m_data(m_buffer.get())
        , m_bytes(bytes)
    {
    }

    //! Constructor for handing over a buffer `data` which was 'array-like'-allocated by `allocator`.
    explicit BinaryBuffer(std::shared_ptr<RawAllocator> allocator,
                          uint8_t* data,
                          std::size_t bytes,
                          uint8_t buffer[],
                          std::size_t allocatedBytes) noexcept
        : m_allocator(allocator)
        , m_buffer(buffer, Deleter{foonathan::memory::make_allocator_reference(*allocator), allocatedBytes})
        , m_allocatedBytes(allocatedBytes)
        , m_data(data)
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

    //! Get the data pointer.
    uint8_t* getData() { return m_data; }
    //! Get the constant data pointer.
    const uint8_t* getData() const { return m_data; }

    //! Get the size in bytes of the current held data.
    std::size_t getSize() const { return m_bytes; }

    //! Check if buffer is empty (nullptr or no bytes)
    bool isEmpty() const { return getSize() == 0 || getData() == nullptr; }

private:
    std::shared_ptr<RawAllocator> m_allocator;  //!< Reference to the allocator.

    /*! Buffer pointer. It is guaranteed by this declaration order 
        that `m_buffer` is destroyed first, and then possibly the `m_allocator`! */
    BufferPtr m_buffer;
    std::size_t m_allocatedBytes = 0;  //!< The current allocated number of bytes in `m_buffer`.

    uint8_t* m_data;          //!< The pointer to the actual data in `m_buffer`.
    std::size_t m_bytes = 0;  //!< The size of the buffer.
};

//! Helper to quickly make a BinaryBuffer, forwards to the constructor.
template<typename RawAllocator, typename... Args>
BinaryBuffer<RawAllocator> makeBinaryBuffer(const std::shared_ptr<RawAllocator>& allocator,
                                            Args&&... args) noexcept
{
    return BinaryBuffer<RawAllocator>{allocator, std::forward<Args>(args)...};
}

#endif
