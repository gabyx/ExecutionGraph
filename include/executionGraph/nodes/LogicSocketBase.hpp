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
#include "executionGraph/common/Assert.hpp"
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
    template<typename IsInput>
    class LogicSocketBase
    {
    private:
        template<typename Data>
        using Socket = meta::if_<IsInput,
                                 LogicSocketInput<Data>,
                                 LogicSocketOutput<Data>>;

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
        SocketIndex index() const noexcept { return m_index; }
        const rttr::type& type() const noexcept { return m_type; }

        const LogicNode& parent() const noexcept
        {
            return *m_parent;
        }

        template<typename T>
        bool isType() const noexcept
        {
            return type() == rttr::type::get<T>();
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
        static constexpr bool isInput() { return IsInput::value; }
        static constexpr bool isOutput() { return !IsInput::value; }

    public:
        //! Cast to a logic socket of type `LogicSocketInput<T>*`.
        //! @throw if `doThrow` or `throwIfBadSocketCast` is `true`
        template<typename Data, bool doThrow = true>
        auto& castToType() noexcept(!doThrow)
        {
            if constexpr(doThrow || throwIfBadSocketCast)
            {
                details::throwSocketCast(!this->template isType<Data>(),
                                         *this,
                                         rttr::type::get<Data>());
            }
            return static_cast<Socket<Data>&>(*this);
        }

        //! Non-const overload.
        template<typename Data, bool doThrow = false>
        auto& castToType() const noexcept(!doThrow)
        {
            return const_cast<const Socket<Data>&>(
                const_cast<LogicSocketBase*>(this)
                    ->castToType<Data, doThrow>());
        }

    protected:
        rttr::type m_type;                    //!< The type of this socket.
        SocketIndex m_index;                  //!< The index of the slot at which this socket is installed in a LogicNode.
        const LogicNode* m_parent = nullptr;  //!< The parent node of of this socket.
    };
}  // namespace executionGraph