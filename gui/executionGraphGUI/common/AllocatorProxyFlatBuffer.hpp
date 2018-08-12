// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Feb 25 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGUI_common_AllocatorProxyFlatBuffer
#define executionGraphGUI_common_AllocatorProxyFlatBuffer

#include <memory>
#include <flatbuffers/flatbuffers.h>
#include "executionGraphGUI/common/BufferPool.hpp"
#include "executionGraphGUI/common/Exception.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    A ligthweight proxy allocator which forwards to another 
    foonathan::RawAllocator. It is compatible with BinaryBuffer, since it allocates
    over `allocate_array`.

    @date Thu Mar 08 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
template<typename TRawAllocator>
class AllocatorProxyFlatBuffer final : public flatbuffers::Allocator
{
public:
    using RawAllocator = TRawAllocator;

public:
    AllocatorProxyFlatBuffer(std::shared_ptr<RawAllocator> allocator)
        : flatbuffers::Allocator{}
        , m_allocator(allocator)
    {}

public:
    //! Allocate `size` bytes of memory.
    virtual uint8_t* allocate(std::size_t bytes) override
    {
        EXECGRAPHGUI_THROW_IF(m_buffer != nullptr,
                              "Another allocation from flatbuffers happened."
                              "Since we cache only one allocation here, this is going to get no good!")
        m_allocatedBytes     = bytes;
        m_enableDeallocation = true;
        m_buffer             = static_cast<uint8_t*>(foonathan::memory::allocator_traits<RawAllocator>::allocate_array(*m_allocator,
                                                                                                           bytes,
                                                                                                           sizeof(uint8_t),
                                                                                                           alignof(uint8_t)));
        // we dont get alignment requirement, its not important.
        return m_buffer;
    }

    // Deallocate `size` bytes of memory at `p` allocated by this allocator.
    virtual void deallocate(uint8_t* p, size_t bytes) override
    {
        if(m_enableDeallocation)
        {
            foonathan::memory::allocator_traits<RawAllocator>::deallocate_array(*m_allocator,
                                                                                p,
                                                                                bytes,
                                                                                sizeof(uint8_t),
                                                                                alignof(uint8_t));
            m_buffer = nullptr;
        }
    }

    //! Disable the deallocation (only rewrap a flatbuffers::DetachedBuffer in our Container)
    //! This function call is dangerous, since memory leaks can happen while an exception
    //! is thrown during rewrapping!
    void disableDeallocation()
    {
        m_enableDeallocation = false;
    }
    //! Return the wrapped allocator.
    auto& getAllocator() const { return m_allocator; }

    //! Get current allocated buffer.
    uint8_t* getCachedAllocatedBuffer() { return m_buffer; }
    //! Return number of allocated bytes.
    std::size_t getCachedAllocatedBytes() { return m_allocatedBytes; }

private:
    bool m_enableDeallocation    = true;        //!< If deallocation is enabled.
    std::size_t m_allocatedBytes = 0;           //!< The number of currently allocated bytes.
    uint8_t* m_buffer            = nullptr;     //!< The current allocated buffer.
    std::shared_ptr<RawAllocator> m_allocator;  //!< The allocator to which allocation/deallocation is forwarded.
};

//! Helper to quickly make a BinaryBuffer, forwards to the constructor and injects the allocated bytes!.
template<typename RawAllocator, typename... Args>
BinaryBuffer<RawAllocator> makeBinaryBuffer(AllocatorProxyFlatBuffer<RawAllocator>&& allocator,
                                            flatbuffers::DetachedBuffer&& buffer,
                                            Args&&... args) noexcept
{
    allocator.disableDeallocation();  // disable deallocation such that DTOR of `buffer` does not deallocate the buffer.
    return makeBinaryBuffer(allocator.getAllocator(),
                            buffer.data(),
                            buffer.size(),
                            allocator.getCachedAllocatedBuffer(),
                            allocator.getCachedAllocatedBytes());
}

#endif
