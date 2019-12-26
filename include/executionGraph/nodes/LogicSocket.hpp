//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 08 2018
//!  @author Gabriel Nützi, <gnuetzi (at) gmail (døt) com>
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#pragma once

#include <unordered_set>
#include <vector>
#include <meta/meta.hpp>
#include <rttr/type>

#include "executionGraph/common/Assert.hpp"
#include "executionGraph/common/DemangleTypes.hpp"
#include "executionGraph/common/EnumClassHelper.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraph/nodes/LogicNode.hpp"

namespace executionGraph
{
    //! The socket base class for all input/output sockets of a node.
    class LogicSocketBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();

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

    public:
        inline SocketIndex getIndex() const noexcept { return m_index; }
        inline const rttr::type& type() const noexcept { return m_type; }

        inline const LogicNode& parent() const noexcept { return *m_parent; }

        template<typename T>
        bool isType() const noexcept { return type() == rttr::type::get<T>(); }

    protected:
        rttr::type m_type;                    //!< The type of this socket.
        SocketIndex m_index;                  //!< The index of the slot at which this socket is installed in a LogicNode.
        const LogicNode* m_parent = nullptr;  //!< The parent node of of this socket.
    };

    //! The input socket base class for all input sockets of a node.
    class LogicSocketInputBase : public LogicSocketBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();

        template<typename... Args>
        LogicSocketInputBase(Args&&... args) noexcept
            : LogicSocketBase(std::forward<Args>(args)...)
        {
        }

        ~LogicSocketInputBase() noexcept;

        //! Cast to a logic socket of type `LogicSocketInput<T>*`.
        //! The cast fails at runtime if the data type `T` does not match!
        template<typename T>
        auto& castToType() const noexcept
        {
            EXECGRAPH_LOGTHROW_IF(!this->template isType<T>(),
                                  "Casting socket index '{0}' with type index '{1}' into type"
                                  "'{2}' of node id: '{3}' which is wrong!",
                                  this->getIndex(),
                                  this->type(),
                                  rttr::type::get<T>().get_name(),
                                  this->parent().getId());

            return static_cast<const LogicSocketInput<T>&>(*this);
        }

        //! Non-const overload.
        template<typename T>
        auto& castToType() noexcept(!throwIfBadSocketCast)
        {
            return const_cast<LogicSocketInput<T>&>(static_cast<LogicSocketInputBase const*>(this)->castToType<T>());
        }

        const LogicNodeDataBase* dataNode() { return m_dataNode; }

    protected:
        const LogicNodeDataBase* m_dataNode = nullptr;  //! Connected data node.
    };

    //! The input socket base class for all input/output sockets of a node.
    class LogicSocketOutputBase : public LogicSocketBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();

    protected:
        template<typename... Args>
        LogicSocketOutputBase(Args&&... args)
            : LogicSocketBase(std::forward<Args>(args)...)
        {
        }

        ~LogicSocketOutputBase() noexcept;

        LogicSocketOutputBase(LogicSocketOutputBase&&) = default;
        LogicSocketOutputBase& operator=(LogicSocketOutputBase&&) = default;

    public:
        //! Cast to a logic socket of type `LogicSocketOutput`<T>*.
        //! The cast fails at runtime if the data type `T` does not match!
        template<typename T>
        auto& castToType() const noexcept
        {
            if constexpr(throwIfBadSocketCast)
            {
                EXECGRAPH_LOGTHROW_IF(this->m_type != rttr::type::get<T>(),
                                      "Casting socket index '{0}' with type index '{1}' into "
                                      "'{2}' of node id: '{3}' which is wrong!",
                                      this->getIndex(),
                                      this->type(),
                                      rttr::type::get<T>().get_name(),
                                      this->parent().getId());
            }

            return static_cast<const LogicSocketOutput<T>&>(*this);
        }

        //! Non-const overload.
        template<typename T>
        auto* castToType()
        {
            return const_cast<LogicSocketOutput<T>&>(static_cast<LogicSocketOutputBase const*>(this)->castToType<T>());
        }
    };

    template<typename TData>
    class LogicSocketInput final : public LogicSocketInputBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();
        using Data = TData;
        using NodeData = LogicNodeData<Data>;

        template<typename... Args>
        LogicSocketInput(Args&&... args)
            : LogicSocketInputBase(rttr::type::get<Data>(), std::forward<Args>(args)...)
        {
        }

        ~LogicSocketInput()
        {
            if(m_dataNode)
            {
                const_cast<NodeData&>(nodeData).onRemoveGetLink(*this);
            }
        }

        //! Copy not allowed (since parent pointer)
        LogicSocketInput(const LogicSocketInput& other) = delete;
        LogicSocketInput& operator=(const LogicSocketInput& other) = delete;

        //! Move allowed
        LogicSocketInput(LogicSocketInput&& other) = default;
        LogicSocketInput& operator=(LogicSocketInput&& other) = default;

        //! Connect a data node.
        bool connect(const NodeData& nodeData)
        {
            m_dataNode = &nodeData;
            const_expr<NodeData*>(m_dataNode)->onAddGetLink(*this);
        }

        //! Disconnect the data node.
        bool disconnect()
        {
            const_expr<NodeData*>(m_dataNode)->onRemoveGetLink(*this);
            m_dataNode = nullptr;
        }

    auto data() { }
    
    const NodeData* dataNode() { return m_dataNode; }

    protected:
        const NodeData* m_dataNode = nullptr;  //! Connected data node.
    };

    template<typename TData>
    class LogicSocketOutput final : public LogicSocketOutputBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();
        using Data = TData;

        template<typename... Args>
        LogicSocketOutput(Args&&... args)
            : LogicSocketOutputBase(rttr::type::get<Data>(), std::forward<Args>(args)...)
        {
        }

        //! Copy not allowed (since parent pointer)
        LogicSocketOutput(const LogicSocketOutput& other) = delete;
        LogicSocketOutput& operator=(const LogicSocketOutput& other) = delete;

        //! Move allowed
        LogicSocketOutput(LogicSocketOutput&& other) = default;
        LogicSocketOutput& operator=(LogicSocketOutput&& other) = default;

           //! Connect a data node.
        bool connect(NodeData& nodeData)
        {
            m_dataNode = &nodeData;
            m_dataNode->onAddGetLink(*this);
        }

        //! Disconnect the data node.
        bool disconnect()
        {
            m_dataNode->onRemoveGetLink(*this);
            m_dataNode = nullptr;
        }
    
        NodeData* dataNode() { return m_dataNode; }

    protected:
        NodeData* m_dataNode = nullptr;  //! Connected data node.
    };

}  // namespace executionGraph