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
#include "executionGraph/nodes/LogicCommon.hpp"

namespace executionGraph
{
    template<typename>
    class LogicNodeData;

    class LogicNodeDataBase
    {
        template<typename>
        friend class LogicNodeData;

    public:
        EG_DEFINE_TYPES();

        auto type() const noexcept { return m_type; }

        template<typename T>
        bool isType() const noexcept
        {
            return type() == rttr::type::get<T>();
        }

        //! Cast to a logic data node of type `LogicNodeData<T>*`.
        //! @throw if `doThrow` or `throwIfBadSocketCast` is `true`
        template<typename T, bool doThrow = false>
        auto& castToType() const noexcept(false)
        {
            if constexpr(doThrow || throwIfNodeDataNoStorage)
            {
                EG_LOGTHROW_IF(!isType<T>(),
                               "Casting node data with id '{0}' and type '{1}' into type "
                               "'{2}' which is wrong!",
                               id(),
                               type().get_name(),
                               rttr::type::get<T>().get_name());
            }

            return static_cast<const LogicNodeData<T>&>(*this);
        }

        //! Non-const overload.
        template<typename T, bool doThrow = false>
        auto& castToType() noexcept(false)
        {
            return const_cast<LogicNodeData<T>&>(
                static_cast<const LogicNodeDataBase*>(this)->castToType<T, doThrow>());
        }

    public:
        virtual void connect(LogicSocketInputBase& inputSocket) noexcept(false)   = 0;
        virtual void connect(LogicSocketOutputBase& outputSocket) noexcept(false) = 0;
        virtual void disconnect(LogicSocketInputBase& inputSocket) noexcept   = 0;
        virtual void disconnect(LogicSocketOutputBase& outputSocket) noexcept = 0;

    public:
        NodeDataId id() const noexcept { return m_id; }
        void setId(NodeDataId id) noexcept { m_id = id; }

    protected:
        LogicNodeDataBase(rttr::type type,
                          NodeDataId id)
            : m_type(type)
            , m_id(id)
        {}

        virtual ~LogicNodeDataBase() = default;

    private:
        const rttr::type m_type;              //!< The type of this node.
        NodeDataId m_id = nodeDataIdInvalid;  //!< Id of this node.
    };
}  // namespace executionGraph