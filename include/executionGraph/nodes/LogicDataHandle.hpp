// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Thu Dec 26 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <type_traits>
#include <meta/meta.hpp>
#include "executionGraph/common/AllocatorUtils.hpp"
#include "executionGraph/common/Assert.hpp"
#include "executionGraph/common/SfinaeMacros.hpp"
#include "executionGraph/common/TupleUtil.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
        Sepcific data handle for `LogicDataNode<T>`.

        @date Fri Dec 27 2019
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TData>
    class LogicDataHandle final
    {
    public:
        static constexpr bool isReadOnly = std::is_const_v<TData>;

        using Data    = TData;
        using RawData = std::remove_const_t<Data>;

    private:
        class ILogicDataHandle
        {
        public:
            virtual ~ILogicDataHandle() noexcept = default;

        public:
            //! Access to memory of the underlying data.
            virtual Data* data() noexcept = 0;
        };

        //! Type-erased wrapper, to allow any type.
        template<typename T>
        class Wrapper final : public ILogicDataHandle
        {
        public:
            Wrapper(T object) noexcept
                : m_object(std::move(object))
            {}

        public:
            Data* data() noexcept override
            {
                return m_object.data();
            }

        private:
            T m_object;
        };

    private:
        template<typename T>
        static constexpr bool constructibleFrom = std::is_same_v<std::remove_const_t<Data>,
                                                                 std::remove_const_t<T>> &&
                                                  (isReadOnly || !std::is_const_v<T>);

        template<typename Handle,
                 typename T = std::remove_pointer_t<decltype(std::declval<Handle>().data())>>
        static constexpr bool constructibleFromHandle = constructibleFrom<T>;

    public:
        LogicDataHandle() noexcept = default;
        explicit LogicDataHandle(std::nullptr_t) noexcept
            : LogicDataHandle(){};

        //! Construct from other handle.
        template<typename Handle,
                 EG_ENABLE_IF(std::is_rvalue_reference_v<Handle&&>),
                 EG_ENABLE_IF(constructibleFromHandle<Handle>)>
        LogicDataHandle(Handle&& handle) noexcept
        {
            auto h   = std::make_unique<Wrapper<Handle>>(std::move(handle));
            m_data   = h->data();
            m_handle = std::move(h);
        }

        template<typename Handle, typename... Args, typename Allocator>
        static create(Allocator alloc, Args&&... args)
        {
            auto h = 
            m_data   = h->data();
            m_handle = std::move(h);
        }

        //! @todo Implement here an allocator constructor
        //! with std::unique_ptr<T, std::function<void(void*)>

        //! Copy-construction not allowed.
        LogicDataHandle(const LogicDataHandle&) = delete;
        //! Copy-assignment not allowed.
        LogicDataHandle& operator=(const LogicDataHandle&) = delete;

        //! Move constructor.
        template<typename T,
                 EG_ENABLE_IF(constructibleFrom<T>)>
        LogicDataHandle(LogicDataHandle<T>&& handle) noexcept
        {
            *this = std::move(handle);
        }

        //! Move assignment.
        template<typename T,
                 EG_ENABLE_IF(constructibleFrom<T>)>
        LogicDataHandle& operator=(LogicDataHandle<T>&& handle) noexcept
        {
            m_handle      = std::move(handle.m_handle);
            m_data        = handle.m_data;
            handle.m_data = nullptr;
            return *this;
        }

    public:
        bool operator==(std::nullptr_t) noexcept
        {
            return m_data == nullptr;
        }

        bool operator!=(std::nullptr_t) noexcept
        {
            return !(*this == nullptr);
        }

    public:
        //! Convert to bool.
        operator bool() noexcept
        {
            return m_data != nullptr;
        }

    public:
        //! Data-access.
        auto& operator*() const noexcept
        {
            return *m_data;
        }

        //! Data-access.
        auto* operator->() const noexcept
        {
            return m_data;
        }

        auto& get() const noexcept
        {
            return *m_data;
        }

    private:
        UniquePtrErased<ILogicDataHandle> m_handle;  //! The type-erased data handle.
        Data* m_data = nullptr;                      //! The actual data.
    };

    //! Invoke a function `f` with all data handles values.
    template<typename... Handles,
             typename F,
             EG_ENABLE_IF((... && meta::is_v<naked<Handles>, LogicDataHandle>))>
    decltype(auto) invoke(const std::tuple<Handles...>& handles,
                          F&& f)
    {
        return tupleUtil::invoke(
            handles,
            [&](const auto&... handles) {
                return f(handles.get()...);
            });
    }
}  // namespace executionGraph