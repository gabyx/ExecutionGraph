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
#include <foonathan/memory/fallback_allocator.hpp>
#include <foonathan/memory/memory_pool.hpp>
#include <foonathan/memory/memory_pool_collection.hpp>
#include <foonathan/memory/smart_ptr.hpp>
#include <foonathan/memory/static_allocator.hpp>
#include <executionGraph/common/AllocatorUtils.hpp>
#include "TestFunctions.hpp"

using namespace foonathan::memory;

// A ref counted class (CefRefPtr from Chromium Embedded Framework)
struct A
{
    A(int a)
        : m_a{a} {}
    template<typename D>
    A(int a, D&& d)
        : m_a{a}, deleter(d)
    {}

    ~A() { std::cout << "dtor A :" << m_a[0] << std::endl; }

    int m_a[100];

    std::function<void(A*)> deleter;  // Wrap here the deleter to delete over the pool
    void refCountDroppedToZero()
    {
        std::cout << "refcount = 0 :";
        deleter(this);  // do a delete over this deleter, calls dtor and deallocates
    }
};

EG_TEST(MemoryPool, Test1)
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

EG_TEST(MemoryPool, Test2)
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

EG_TEST(MemoryPool, AnyAllocator)
{
    bool dtorCalled = false;
    struct A
    {
        virtual ~A() {}
    };
    struct B : public A
    {
        B(bool& d)
            : dtorCalled(d) {}
        ~B()
        {
            dtorCalled = true;
        }
        bool& dtorCalled;
        int a[1024 * 1024 * 32];
    };

    using namespace literals;
    using Poolocator = memory_pool_collection<node_pool, log2_buckets>;
    using RawAllocTh = thread_safe_allocator<Poolocator>;

    Poolocator alloc(1_GiB, 4_GiB);
    auto spAlloc = std::make_shared<RawAllocTh>(std::move(alloc));

    using namespace executionGraph;

    // Make type-erased unique_ptr with state-full allocator
    {
        // auto spB = allocatorUtils::makeUniqueErased<B>(spAlloc, dtorCalled);
        // ASSERT_EQ(spB->dtorCalled, false);
        // UniquePtrErased<A> spA = std::move(spB);
        // ASSERT_TRUE(spA.get() != nullptr);
        // spA = nullptr;
        // ASSERT_EQ(dtorCalled, true);
    }

    {
        auto spB = allocatorUtils::makeUniqueErased<B>(heap_allocator{}, dtorCalled);
    }
}

EG_TEST(MemoryPool, TestingDefaultCTORDeleter)
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