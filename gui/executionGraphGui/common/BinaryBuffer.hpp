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
#include <memory>
#include <foonathan/memory/smart_ptr.hpp>
#include "executionGraphGui/common/Assert.hpp"

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
    //! Constructor for a meaningful buffer with size `bytes` allocated by `allocator`.
    BinaryBuffer(std::shared_ptr<RawAllocator> allocator,
                 std::size_t bytes = 0)
        : m_allocator(allocator)
    {
        reserve(bytes);
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
    {}

    //! Copy prohibited.
    BinaryBuffer(const BinaryBuffer&) = delete;
    BinaryBuffer& operator=(const BinaryBuffer&) = delete;

    //! Move-construct the buffer. 
    //! Buffer `o` stays valid and can be reused again.
    BinaryBuffer(BinaryBuffer&& o)
    {
        *this = std::move(o);
    };
    //! Move-assign to the buffer. 
    //! Buffer `o` stays valid and can be reused again.
    BinaryBuffer& operator=(BinaryBuffer&& o)
    {
        m_buffer         = std::move(o.m_buffer);
        m_allocator      = o.m_allocator;  // no move: beacause it would null o.m_allocator
        m_allocatedBytes = o.m_allocatedBytes;
        m_bytes          = o.m_bytes;
        m_data           = o.m_data;

        o.clear();
        return *this;
    };

    //! Begin/End Iterators
    //@{
    iterator begin() { return data(); }
    const_iterator begin() const { return data(); }
    const_iterator cbegin() const { return begin(); }
    iterator end() { return data() + size(); }
    const_iterator end() const { return data() + size(); }
    const_iterator cend() const { return end(); }
    //@}

    //! Resize the buffer to containe `bytes` bytes.
    void resize(std::uint64_t bytes)
    {
        reserve(bytes);
        m_bytes = bytes;
    }

    //! Make sure the allocated bytes are at least `bytes`.
    void reserve(std::uint64_t bytes)
    {
        if(bytes > allocatedSize())
        {
            reallocate(bytes);
        }
    }

    //! Clears the whole buffer (allocator stays valid).
    void clear()
    {
        m_buffer         = nullptr;
        m_allocatedBytes = 0;
        m_bytes          = 0;
        m_data           = 0;
    }

    //! Get the data pointer.
    uint8_t* data() { return m_data; }
    //! Get the constant data pointer.
    const uint8_t* data() const { return m_data; }

    //! Get the size in bytes of the current held data.
    std::uint64_t size() const { return m_bytes; }

    //! Get the size in bytes of the current allocated memory.
    std::uint64_t allocatedSize() const { return m_allocatedBytes; }

    //! Check if buffer is empty (nullptr or no bytes)
    bool isEmpty() const { return size() == 0 || data() == nullptr; }

private:
    //! Allocates a buffer with size `bytes` and copies over the current bytes to the new buffer.
    void reallocate(std::uint64_t bytes)
    {
        EXECGRAPHGUI_ASSERT(bytes > 0, "Invalid size!");
        // allocate (`allocate_unique` only captures allocator by reference, there we store the allocator here)
        auto buffer = foonathan::memory::allocate_unique<uint8_t[]>(*m_allocator, bytes);
        // copy data m_buffer -> buffer
        auto bytesToCopy = std::min(size(), bytes);
        if(bytesToCopy)
        {
            std::memcpy(buffer.get(), m_buffer.get(), bytesToCopy);
        }

        // reset state
        m_buffer         = std::move(buffer);
        m_allocatedBytes = bytes;
        m_data           = m_buffer.get();
    }

private:
    std::shared_ptr<RawAllocator> m_allocator;  //!< Reference to the allocator.

    /*! Buffer pointer. It is guaranteed by this declaration order 
        that `m_buffer` is destroyed first, and then possibly the `m_allocator`! */
    BufferPtr m_buffer;
    std::uint64_t m_allocatedBytes = 0;  //!< The current allocated number of bytes in `m_buffer`.

    uint8_t* m_data;            //!< The pointer to the actual data in `m_buffer`.
    std::uint64_t m_bytes = 0;  //!< The size of the buffer.
};

//! Helper to quickly make a BinaryBuffer, forwards to the constructor.
template<typename RawAllocator, typename... Args>
BinaryBuffer<RawAllocator> makeBinaryBuffer(const std::shared_ptr<RawAllocator>& allocator,
                                            Args&&... args) noexcept
{
    return BinaryBuffer<RawAllocator>{allocator, std::forward<Args>(args)...};
}

#endif
