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
        // we dont get alignment requirement, its not important.
        return static_cast<uint8_t*>(foonathan::memory::allocator_traits<RawAllocator>::allocate_array(*m_allocator,
                                                                                                       bytes,
                                                                                                       sizeof(uint8_t),
                                                                                                       alignof(uint8_t)));
    }

    // Deallocate `size` bytes of memory at `p` allocated by this allocator.
    virtual void deallocate(uint8_t* p, size_t bytes) override
    {
        foonathan::memory::allocator_traits<RawAllocator>::deallocate_array(*m_allocator,
                                                                            p,
                                                                            bytes,
                                                                            sizeof(uint8_t),
                                                                            alignof(uint8_t));
    }

    //! Return the wrapped allocator.
    auto& getAllocator() const { return m_allocator; }

private:
    std::shared_ptr<RawAllocator> m_allocator;  //!< The allocator to which allocation/deallocation is forwarded.
};

//! Helper to quickly make a BinaryBuffer, releases the memory block from the FlatBufferBuilder
//! and transfers ownership.
template<typename RawAllocator, typename... Args>
BinaryBuffer<RawAllocator> releaseIntoBinaryBuffer(AllocatorProxyFlatBuffer<RawAllocator>&& allocator,
                                                   flatbuffers::FlatBufferBuilder& builder,
                                                   Args&&... args) noexcept
{
    std::size_t size, offset;
    uint8_t* data = builder.ReleaseRaw(size, offset);
    return makeBinaryBuffer(allocator.getAllocator(),
                            data + offset,
                            size - offset,
                            data,
                            size);
}

#endif
