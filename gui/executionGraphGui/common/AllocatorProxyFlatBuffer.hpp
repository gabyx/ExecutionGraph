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

#ifndef common_AllocatorProxyFlatBuffer
#define common_AllocatorProxyFlatBuffer

#include <flatbuffers/flatbuffers.h>
#include "common/BufferPool.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    A ligthweight proxy allocator which forwards to another 
    foonathan::RawAllocator. 

    @date Thu Mar 08 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
template<typename TAllocator>
class AllocatorProxyFlatBuffer final : public flatbuffers::Allocator
{
public:
    using Allocator = TAllocator;

public:
    AllocatorProxyFlatBuffer(Allocator& allocator)
        : flatbuffers::Allocator{}
        , m_allocator(allocator)
    {}

public:
    //! Allocate `size` bytes of memory.
    virtual uint8_t* allocate(std::size_t size) override
    {
        // we dont get alignment requirement, its not important.
        return static_cast<uint8_t*>(m_allocator.allocate_node(size, alignof(char)));
    }

    // Deallocate `size` bytes of memory at `p` allocated by this allocator.
    virtual void deallocate(uint8_t* p, size_t size) override
    {
        m_allocator.deallocate_node(p, size, alignof(char));
    }

private:
    Allocator& m_allocator;  //!< The allocator to which allocation/deallocation is forwarded.
};

#endif
