// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Mon Dec 17 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGui_server_HttpFieldAllocator_hpp
#define executionGraphGui_server_HttpFieldAllocator_hpp

#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <executionGraphGui/common/Exception.hpp>

namespace details
{
    struct StaticPool
    {
    public:
        std::size_t size_;
        std::size_t refs_  = 1;
        std::size_t count_ = 0;
        char* p_;

        char* end()
        {
            return reinterpret_cast<char*>(this + 1) + size_;
        }

        explicit StaticPool(std::size_t size)
            : size_(size)
            , p_(reinterpret_cast<char*>(this + 1))
        {
        }

    public:
        static StaticPool&
        construct(std::size_t size)
        {
            auto p = new char[sizeof(StaticPool) + size];
            return *(::new(p) StaticPool{size});
        }

        StaticPool& share()
        {
            ++refs_;
            return *this;
        }

        void destroy()
        {
            if(refs_--)
                return;
            this->~StaticPool();
            delete[] reinterpret_cast<char*>(this);
        }

        void* alloc(std::size_t n)
        {
            auto last = p_ + n;
            if(last >= end())
            {
                EXECGRAPHGUI_THROW_TYPE(std::bad_alloc);
            }
            ++count_;
            auto p = p_;
            p_     = last;
            return p;
        }

        void dealloc()
        {
            if(--count_)
                return;
            p_ = reinterpret_cast<char*>(this + 1);
        }
    };
}  // namespace details

/** A non-thread-safe allocator optimized for @ref basic_fields.

    This allocator obtains memory from a pre-allocated memory block
    of a given size. It does nothing in deallocate until all
    previously allocated blocks are deallocated, upon which it
    resets the internal memory block for re-use.

    To use this allocator declare an instance persistent to the
    connection or session, and construct with the block size.
    A good rule of thumb is 20% more than the maximum allowed
    header size. For example if the application only allows up
    to an 8,000 byte header, the block size could be 9,600.

    Then, for every instance of `message` construct the header
    with a copy of the previously declared allocator instance.
*/
template<typename T>
class HttpFieldAllocator
{
public:
    details::StaticPool* m_pool;

public:
    using value_type      = T;
    using is_always_equal = std::false_type;
    using pointer         = T*;
    using reference       = T&;
    using const_pointer   = T const*;
    using const_reference = T const&;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    template<class U>
    struct rebind
    {
        using other = HttpFieldAllocator<U>;
    };

    explicit HttpFieldAllocator(std::size_t size)
        : m_pool(&details::StaticPool::construct(size))
    {
    }

    HttpFieldAllocator(HttpFieldAllocator const& other)
        : m_pool(&other.m_pool->share())
    {
    }

    template<class U>
    HttpFieldAllocator(HttpFieldAllocator<U> const& other)
        : m_pool(&other.m_pool->share())
    {
    }

    ~HttpFieldAllocator()
    {
        m_pool->destroy();
    }

    value_type* allocate(size_type n)
    {
        return static_cast<value_type*>(
            m_pool->alloc(n * sizeof(T)));
    }

    void deallocate(value_type*, size_type)
    {
        m_pool->dealloc();
    }

    template<class U>
    friend bool
    operator==(HttpFieldAllocator const& lhs,
               HttpFieldAllocator<U> const& rhs)
    {
        return &lhs.m_pool == &rhs.m_pool;
    }

    template<class U>
    friend bool
    operator!=(HttpFieldAllocator const& lhs,
               HttpFieldAllocator<U> const& rhs)
    {
        return !(lhs == rhs);
    }
};

#endif