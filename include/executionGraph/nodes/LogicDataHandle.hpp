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
#include "executionGraph/common/Assert.hpp"
#include "executionGraph/common/SfinaeMacros.hpp"
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
        friend class LogicSocketData<RawData>;

    private:
        template<typename T>
        static constexpr bool constructibleFrom = std::is_same_v<std::remove_const_t<Data>,
                                                                 std::remove_const_t<T>> &&
                                                  (isReadOnly || !std::is_const_v<T>);

    public:
        LogicDataHandle() noexcept = default;
        explicit LogicDataHandle(std::nullptr_t) noexcept
            : LogicDataHandle(){};

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
            m_data        = handle.m_data;
            handle.m_data = nullptr;
            return *this;
        }

    private:
        //! Construct from reference to a data value `data`.
        template<typename T,
                 EG_ENABLE_IF(constructibleFrom<T>)>
        explicit LogicDataHandle(T& data) noexcept
            : m_data(&data)
        {}

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
        Data* m_data = nullptr;
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