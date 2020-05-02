// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Apr 18 2020
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <foonathan/memory/allocator_storage.hpp>
#include <foonathan/memory/allocator_traits.hpp>
#include <foonathan/memory/default_allocator.hpp>
#include <foonathan/memory/segregator.hpp>
#include <foonathan/memory/static_allocator.hpp>
#include <meta/meta.hpp>
#include <executionGraph/common/AnyInvocable.hpp>
#include <executionGraph/common/SfinaeMacros.hpp>
#include <executionGraph/common/StaticAssert.hpp>

namespace executionGraph
{
    namespace memory = foonathan::memory;

    //! @todo We need here a move-only function since allocators are only movable!
    template<typename T>
    using UniquePtrErased = std::unique_ptr<T, AnyInvocable<void(void*)>>;

    namespace memoryUtils
    {
        //! The stateless default allocator.
        using DefaultAllocator = memory::default_allocator;

        namespace details
        {
            template<typename T>
            using isSharedPtr = meta::is<std::remove_cvref_t<T>, std::shared_ptr>;

            template<typename AllocHandle,
                     EG_ENABLE_IF(isSharedPtr<AllocHandle>::value)>
            auto& get(const AllocHandle& handle) noexcept
            {
                return *handle;
            }

            template<typename AllocHandle,
                     EG_ENABLE_IF(!isSharedPtr<AllocHandle>::value)>
            auto& get(AllocHandle& handle) noexcept
            {
                return handle;
            }
        }  // namespace details

        //! Similar to `std::make_unique<T>` but allocates with a `RawAllocator`
        //! and type-erases the allocator in the deleter of `std::unique_ptr`.
        //! @param alloc A `RawAlloctator` or a `std::shared_ptr<RawAllocator>`.
        //!              which will be moved into the deleter.
        //!              If the allocator is state-full it needs to be moved
        //!              into `alloc` which is correct,
        //!              if state-less it can be copied or moved into `alloc`.
        //! @return std::unique_ptr<T,...> where the allocator has
        //!         been captured in the type-erased deleter.
        template<typename T,
                 typename RawAllocator,
                 typename... Args>
        UniquePtrErased<T> makeUniqueErased(RawAllocator alloc,
                                            Args&&... args)
        {
            using Allocator = std::remove_cvref_t<decltype(details::get(*static_cast<RawAllocator*>(0)))>;
            EG_STATIC_ASSERT(memory::is_raw_allocator<Allocator>::value, "Allocator needs to be a RawAllocator");
            EG_STATIC_ASSERT(!std::is_const_v<Allocator>, "Allocator needs to be non-const");

            using Traits = memory::allocator_traits<Allocator>;
            void* node   = Traits::allocate_node(details::get(alloc), sizeof(T), alignof(T));

            // RawPtr deallocates memory in case of constructor exception below
            UniquePtrErased<T>
                result(static_cast<T*>(node),
                       [&alloc](void* object) mutable {
                           Traits::deallocate_node(details::get(alloc), object, sizeof(T), alignof(T));
                       });

            // Call constructor.
            ::new(node) T(std::forward<Args>(args)...);

            // Pass ownership to return value
            // using a deleter that calls the destructor and deallocates
            // The allocator (either smart-pointer like or by value)
            // is moved and  captured by value!
            return UniquePtrErased<T>{
                result.release(),
                [al = std::move(alloc)](void* object) mutable {
                    // Allocator `alloc`'s life continues in this lambda...
                    static_cast<T*>(object)->~T();
                    Traits::deallocate_node(details::get(al), object, sizeof(T), alignof(T));
                }};
        }

        //! Exactly the same as `makeUniqueErased` except that an small buffer `sboStorage`
        //! is used for Small Buffer Optimization (SBO) inside a segregator built from `alloc`
        //! and a `static_allocator`.
        template<typename T,
                 typename RawAllocator,
                 auto N,
                 typename... Args>
        UniquePtrErased<T> makeUniqueErasedSBO(RawAllocator alloc,
                                               memory::static_allocator_storage<N>& sboStorage,
                                               Args&&... args)
        {
            using Allocator = std::remove_cvref_t<decltype(details::get(*static_cast<RawAllocator*>(0)))>;
            EG_STATIC_ASSERT(memory::is_raw_allocator<Allocator>::value, "Allocator needs to be a RawAllocator");
            EG_STATIC_ASSERT(!std::is_const_v<Allocator>, "Allocator needs to be non-const");

            auto sboSize = sizeof(sboStorage.storage);

            // Attention: `alloc` is reference captured in this `segAlloc`.
            // `segAlloc` and `alloc` both need to have the same lifetime!
            // `memory::static_allocator` uses a debug fence before/after,
            // therefore the threshold needs to adapt!
            auto segAlloc = memory::make_segregator(memory::threshold(sboSize - 2 * FOONATHAN_MEMORY_DEBUG_FENCE,
                                                                      memory::static_allocator(sboStorage)),
                                                    memory::make_allocator_reference(details::get(alloc)));

            using Traits = memory::allocator_traits<decltype(segAlloc)>;
            void* node   = Traits::allocate_node(segAlloc, sizeof(T), alignof(T));

            // RawPtr deallocates memory in case of constructor exception below
            UniquePtrErased<T>
                result(static_cast<T*>(node),
                       [&segAlloc](void* object) mutable {
                           Traits::deallocate_node(segAlloc, object, sizeof(T), alignof(T));
                       });

            // Call constructor.
            ::new(node) T(std::forward<Args>(args)...);

            // Pass ownership to return value
            // using a deleter that calls the destructor and deallocates
            // The allocator (either smart-pointer like or by value)
            // is moved and  captured by value!
            return UniquePtrErased<T>{
                result.release(),
                [al       = std::move(alloc),
                 segAlloc = std::move(segAlloc)](void* object) mutable {
                    // Allocator `alloc`'s and `segAlloc`'s life continue in this lambda...
                    static_cast<T*>(object)->~T();
                    Traits::deallocate_node(segAlloc, object, sizeof(T), alignof(T));
                }};
        }

    }  // namespace memoryUtils
}  // namespace executionGraph