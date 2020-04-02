// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Jan 11 2020
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <rttr/type>
#include "executionGraph/common/MetaCommon.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"

namespace executionGraph
{
    namespace details
    {
        void EG_EXPORT throwSocketCast(bool assert,
                                       const LogicSocketInputBase& socket,
                                       const rttr::type& type);
        void EG_EXPORT throwSocketCast(bool assert,
                                       const LogicSocketOutputBase& socket,
                                       const rttr::type& type);
    }  // namespace details

    //! The socket base class.
    class LogicSocketBase
    {
    public:
        EG_DEFINE_TYPES();

        LogicSocketBase(LogicSocketBase&&) = default;
        LogicSocketBase& operator=(LogicSocketBase&&) = default;

    protected:
        LogicSocketBase(rttr::type type,
                        SocketIndex index,
                        const LogicNode& parent) noexcept
            : m_type(type)
            , m_index(index)
            , m_parent(&parent)
        {
        }

        ~LogicSocketBase() = default;

    public:
        inline SocketIndex index() const noexcept { return m_index; }
        inline const rttr::type& type() const noexcept { return m_type; }

        inline const LogicNode& parent() const noexcept { return *m_parent; }

        template<typename T>
        bool isType() const noexcept
        {
            return type() == rttr::type::get<T>();
        }

    protected:
        rttr::type m_type;                    //!< The type of this socket.
        SocketIndex m_index;                  //!< The index of the slot at which this socket is installed in a LogicNode.
        const LogicNode* m_parent = nullptr;  //!< The parent node of of this socket.
    };

    //! The input socket base class.
    class LogicSocketInputBase : public LogicSocketBase
    {
    public:
        EG_DEFINE_TYPES();

    protected:
        template<typename... Args>
        LogicSocketInputBase(Args&&... args) noexcept
            : LogicSocketBase(std::forward<Args>(args)...)
        {
        }

        virtual ~LogicSocketInputBase() noexcept = default;

    public:
        //! Cast to a logic socket of type `LogicSocketInput<T>*`.
        //! @throw if `doThrow` or `throwIfBadSocketCast` is `true`
        template<typename T, bool doThrow = false>
        auto& castToType() const noexcept
        {
            if constexpr(doThrow || throwIfBadSocketCast)
            {
                details::throwSocketCast(!this->template isType<T>(),
                                         *this,
                                         rttr::type::get<T>());
            }
            return static_cast<const LogicSocketInput<T>&>(*this);
        }

        //! Non-const overload.
        template<typename T, bool doThrow = false>
        auto& castToType() noexcept
        {
            return const_cast<LogicSocketInput<T>&>(
                static_cast<LogicSocketInputBase const*>(this)
                    ->castToType<T, doThrow>());
        }

    public:
        template<typename T,
                 EG_ENABLE_IF(std::is_base_of_v<LogicSocketDataBase, naked<T>>)>
        void connect(T& socketData) noexcept(false)
        {
            socketData.connect(*this);
        }

        virtual void disconnect() noexcept = 0;

    public:
        static constexpr bool isInput() { return true; }
        static constexpr bool isOutput() { return false; }
    };

    //! The output socket base class.
    class LogicSocketOutputBase : public LogicSocketBase
    {
    public:
        EG_DEFINE_TYPES();

    protected:
        template<typename... Args>
        LogicSocketOutputBase(Args&&... args)
            : LogicSocketBase(std::forward<Args>(args)...)
        {
        }

        virtual ~LogicSocketOutputBase() noexcept = default;

        LogicSocketOutputBase(LogicSocketOutputBase&&) = default;
        LogicSocketOutputBase& operator=(LogicSocketOutputBase&&) = default;

    public:
        //! Cast to a logic socket of type `LogicSocketOutput`<T>*.
        //! @throw if `doThrow` or `throwIfBadSocketCast` is `true`
        template<typename T, bool doThrow = false>
        auto& castToType() const noexcept(false)
        {
            if constexpr(doThrow || throwIfBadSocketCast)
            {
                details::throwSocketCast(!this->template isType<T>(),
                                         *this,
                                         rttr::type::get<T>());
            }

            return static_cast<const LogicSocketOutput<T>&>(*this);
        }

        //! Non-const overload.
        template<typename T, bool doThrow = false>
        auto& castToType() noexcept(false)
        {
            return const_cast<LogicSocketOutput<T>&>(
                static_cast<LogicSocketOutputBase const*>(this)
                    ->castToType<T, doThrow>());
        }

    public:
        template<typename T,
                 EG_ENABLE_IF(std::is_base_of_v<LogicSocketDataBase, naked<T>>)>
        void connect(T& socketData) noexcept(false)
        {
            socketData.connect(*this);
        }

        virtual void disconnect() noexcept = 0;

    public:
        static constexpr bool isInput() { return false; }
        static constexpr bool isOutput() { return true; }
    };
}  // namespace executionGraph