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

#include <vector>
#include <foonathan/memory/memory_pool.hpp>
#include <foonathan/memory/smart_ptr.hpp>
#include "TestFunctions.hpp"

using namespace foonathan::memory;

// A ref counted class (CefRefPtr from Chromium Embedded Framework)
struct A
{
    A(int a)
        : m_a{a} {}
    template<typename D>
    A(int a, D&& d)
        : m_a{a}, deleter(d) {}

    ~A() { std::cout << "dtor A :" << m_a[0] << std::endl; }

    int m_a[100];

    std::function<void(A*)> deleter;  // Wrap here the deleter to delete over the pool
    void refCountDroppedToZero()
    {
        std::cout << "refcount = 0 :";
        deleter(this);  // do a delete over this deleter, calls dtor and deallocates
    }
};

MY_TEST(MemoryPool, Test1)
{
    using RawAllocator = memory_pool<>;
    using Ptr          = std::unique_ptr<A, allocator_deleter<A, RawAllocator>>;
    using RawPtr       = std::unique_ptr<A, allocator_deallocator<A, RawAllocator>>;

    RawAllocator pool(sizeof(A), sizeof(A) * 10);
    std::vector<Ptr> vec;
    for(auto i = 0; i < 30; ++i)
    {
        auto memory = pool.allocate_node();

        // raw_ptr deallocates memory in case of constructor exception
        RawPtr result(static_cast<A*>(memory), {pool});
        // call constructor (placement new)
        ::new(memory) A(i);
        // pass ownership to return value CefRefPtr which will use the internal BackendRequestHandler::m_deleter
        std::cout << "allocated " << i << std::endl;
        vec.emplace_back(Ptr(result.release(), {pool}));
    }
    std::shuffle(vec.begin(), vec.end(), std::mt19937{});
}

MY_TEST(MemoryPool, Test2)
{
    using RawAllocator = memory_pool<node_pool, new_allocator>;
    using RawPtr       = std::unique_ptr<A, allocator_deallocator<A, RawAllocator>>;

    RawAllocator pool(sizeof(A), sizeof(A) * 10);
    std::vector<A*> vec;
    for(auto i = 0; i < 30; ++i)
    {
        auto memory = pool.allocate_node();

        // raw_ptr deallocates memory in case of constructor exception
        RawPtr result(static_cast<A*>(memory), {pool});
        // call constructor (placement new), insert a deleter into the instance
        ::new(memory) A(i, [&pool](auto* p) { allocator_deleter<A, RawAllocator>{pool}(p); });
        // pass ownership to return value CefRefPtr which will use the internal BackendRequestHandler::m_deleter
        std::cout << "allocated " << i << std::endl;
        vec.emplace_back(result.release());
    }

    std::shuffle(vec.begin(), vec.end(), std::mt19937{});
    for(auto* p : vec)
    {
        p->refCountDroppedToZero();  // delete and deallocate
    }
}

MY_TEST(MemoryPool, Test3)
{
    using RawAllocator = memory_pool<node_pool>;
    using RawPtr       = std::unique_ptr<uint8_t[], allocator_deleter<uint8_t[], RawAllocator>>;
    RawAllocator pool(10, 10 * 10);

    RawPtr ptr(0, {foonathan::memory::make_allocator_reference(pool), 0});

    std::vector<RawPtr> vec;
    for(auto i = 0; i < 30; ++i)
    {
        vec.emplace_back(allocate_unique<uint8_t[]>(pool, 10u));
    }
}

MY_TEST(MemoryPool, TestingDefaultCTORDeleter)
{
    using RawAllocator = memory_pool<node_pool>;
    using Deleter      = allocator_deleter<uint8_t[], RawAllocator>;
    using RawPtr       = std::unique_ptr<uint8_t[], allocator_deleter<uint8_t[], RawAllocator>>;
    Deleter d;
    RawPtr p;
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}