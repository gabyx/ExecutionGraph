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

#include <type_traits>
#include <meta/meta.hpp>
#include <rttr/type>
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/ILogicSocketDataAccess.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
        Base class for any data node.

        @date Wed Apr 01 2020
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    class LogicSocketDataBase
    {
    protected:
        LogicSocketDataBase(rttr::type type, SocketDataId id)
            : m_type(type)
            , m_id(id)
        {}

    public:
        virtual ~LogicSocketDataBase() = default;

        LogicSocketDataBase(const LogicSocketDataBase& other) = default;
        LogicSocketDataBase& operator=(const LogicSocketDataBase& other) = default;

        LogicSocketDataBase(LogicSocketDataBase&& other) = default;
        LogicSocketDataBase& operator=(LogicSocketDataBase&& other) = default;

    protected:
        template<typename Derived>
        void init(Derived& derived)
        {
            static_assert(std::is_base_of_v<ILogicSocketDataAccess<typename Derived::Data>, Derived>,
                          "Derived from this base needs to implement LogicSocketData<Data>");
            m_dataAccess = &derived;
        }

    public:
        EG_DEFINE_TYPES();

        auto type() const noexcept { return m_type; }

        template<typename T>
        bool isType() const noexcept
        {
            return type() == rttr::type::get<T>();
        }

        //! Cast to the data acccess of type `ILogicSocketDataAcccess<T>&`.
        //! @throw if `doThrow` or `throwIfBadSocketCast` is `true`
        template<typename T, bool doThrow = true>
        auto& dataAccess() noexcept(!doThrow)
        {
            EG_ASSERT(m_dataAccess, "Data access not set");

            if constexpr(doThrow || throwIfSocketDataNoStorage)
            {
                EG_LOGTHROW_IF(!isType<T>(),
                               "Casting node data with id '{0}' and type '{1}' into type "
                               "'{2}' which is wrong!",
                               id(),
                               type().get_name(),
                               rttr::type::get<T>().get_name());
            }

            return static_cast<ILogicSocketDataAccess<T>&>(*m_dataAccess);
        }

        //! Non-const overload.
        template<typename T, bool doThrow = true>
        auto& dataAccess() const noexcept(!doThrow)
        {
            return static_cast<const ILogicSocketDataAccess<T>&>(
                const_cast<LogicSocketDataBase*>(this)->dataAccess<T, doThrow>());
        }

    public:
        virtual void connect(LogicSocketInputBase& inputSocket) noexcept(false)      = 0;
        virtual void connect(LogicSocketOutputBase& outputSocket) noexcept(false)    = 0;
        virtual void disconnect(LogicSocketInputBase& inputSocket) noexcept(false)   = 0;
        virtual void disconnect(LogicSocketOutputBase& outputSocket) noexcept(false) = 0;

    public:
        SocketDataId id() const noexcept { return m_id; }
        void setId(SocketDataId id) noexcept { m_id = id; }

    private:
        const rttr::type m_type;                                         //!< The type of this node.
        SocketDataId m_id                        = socketDataIdInvalid;  //!< Id of this node.
        ILogicSocketDataAccessBase* m_dataAccess = nullptr;              //!< Data access.
    };
}  // namespace executionGraph