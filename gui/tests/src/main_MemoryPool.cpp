//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Tue Jan 16 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#include <foonathan/memory/memory_pool.hpp>
#include <foonathan/memory/smart_ptr.hpp>
#include <vector>
#include "TestFunctions.hpp"
#include "include/cef_base.h"

using namespace foonathan::memory;

// A ref counted class (CefRefPtr from Chromium Embedded Framework)
struct A : CefBaseRefCounted
{
    template<typename D>
    A(int a, D&& d)
        : m_a{a}, deleter(d) {}
    ~A() { std::cout << "dtor A :" << m_a[0] << std::endl; }
    int m_a[100];

    std::function<void(A*)> deleter;  // Wrap here the deleter to delete over the pool

    void AddRef() const override { m_refCount.AddRef(); }
    bool Release() const override
    {
        if(m_refCount.Release())
        {
            deleter(const_cast<A*>(this));
            return true;
        }
        return false;
    }
    bool HasOneRef() const override { return m_refCount.HasOneRef(); }

    CefRefCount m_refCount;
};

MY_TEST(MemoryPool, Test1)
{
    using RawAllocator = memory_pool<>;
    using Ptr          = CefRefPtr<A>;
    using RawPtr       = std::unique_ptr<A, allocator_deallocator<A, RawAllocator>>;

    RawAllocator pool(sizeof(A), sizeof(A) * 10);
    std::vector<Ptr> vec;
    for(auto i = 0; i < 30000; ++i)
    {
        auto memory = pool.allocate_node();

        // raw_ptr deallocates memory in case of constructor exception
        RawPtr result(static_cast<A*>(memory), {pool});
        // call constructor (placement new)
        ::new(memory) A(i, [&pool](auto* p) { allocator_deleter<A, RawAllocator>{pool}(p); });
        // pass ownership to return value CefRefPtr which will use the internal BackendRequestHandler::m_deleter
        std::cout << "allocated " << i << std::endl;
        vec.emplace_back(Ptr(result.release()));
    }
    std::shuffle(vec.begin(), vec.end(), std::mt19937{});
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}