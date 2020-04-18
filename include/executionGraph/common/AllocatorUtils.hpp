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
#include <foonathan/memory/allocator_traits.hpp>
#include <meta/meta.hpp>
#include <executionGraph/common/SfinaeMacros.hpp>
#include <executionGraph/common/StaticAssert.hpp>

namespace executionGraph
{
    template<typename T>
    using UniquePtrErased = std::unique_ptr<T, std::function<void(void*)>>;

    namespace allocatorUtils
    {
        namespace details
        {
            template<typename T>
            using isSharedPtr = meta::is<std::remove_cvref_t<T>, std::shared_ptr>;

            template<typename AllocHandle,
                     EG_ENABLE_IF(isSharedPtr<AllocHandle>::value)>
            auto& get(AllocHandle& handle)
            {
                return *handle;
            }

            template<typename AllocHandle,
                     EG_ENABLE_IF(!isSharedPtr<AllocHandle>::value)>
            auto& get(AllocHandle& handle)
            {
                return handle;
            }
        }  // namespace details

        //! Similar to `std::make_unique<T>` but allocates with a `RawAllocator`
        //! and type-erases the allocator in the deleter of `std::unique_ptr`.
        //!
        //! @return std::unique_ptr<T,...> where the allocator has
        //!         been captured in the type-erased deleter
        template<typename T,
                 typename RawAllocator,
                 typename... Args>
        UniquePtrErased<T> makeUniqueErased(RawAllocator alloc, Args&&... args)
        {
            using namespace foonathan;

            using Allocator = meta::if_<details::isSharedPtr<RawAllocator>,
                                        typename RawAllocator::element_type,
                                        RawAllocator>;

            EG_STATIC_ASSERT(!std::is_const_v<Allocator>, "Allocator needs to be non-const");

            EG_STATIC_ASSERT(!memory::allocator_traits<Allocator>::is_stateful::value ||
                                 details::isSharedPtr<RawAllocator>::value,
                             "A stateful RawAllocator needs to be a std::shared_ptr<RawAllocator> "
                             "because we cannot safely type-erase otherwise.");

            using Traits = memory::allocator_traits<Allocator>;
            auto memory  = Traits::allocate_node(details::get(alloc), sizeof(T), alignof(T));

            // RawPtr deallocates memory in case of constructor exception below
            UniquePtrErased<T>
                result(static_cast<T*>(memory),
                       [&alloc](void* object) {
                           Traits::deallocate_node(details::get(alloc), object, sizeof(T), alignof(T));
                       });

            // Call constructor
            ::new(memory) T(std::forward<Args>(args)...);

            // Pass ownership to return value
            // using a deleter that calls the destructor and deallocates
            // The allocator is captured by value!
            return UniquePtrErased<T>{
                result.release(),
                [alloc](void* object) {
                    static_cast<T*>(object)->~T();
                    Traits::deallocate_node(details::get(alloc), object, sizeof(T), alignof(T));
                }};
        }

    }  // namespace allocatorUtils
}  // namespace executionGraph