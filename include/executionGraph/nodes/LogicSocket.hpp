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
#include <rttr/type>

#include "executionGraph/common/Assert.hpp"
#include "executionGraph/common/DemangleTypes.hpp"
#include "executionGraph/common/EnumClassHelper.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraph/nodes/LogicNode.hpp"
#include "executionGraph/nodes/LogicSocketData.hpp"

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
        LogicSocketBase(const rttr::type& type,
                        SocketIndex index,
                        LogicNode& parent)
            : m_type(type)
            , m_index(index)
            , m_parent(parent)
        {
        }

    public:
        inline SocketIndex getIndex() const { return m_index; }
        inline const rttr::type& type() const { return m_type; }

        inline LogicNode& parent() const { return m_parent; }

        template<typename T>
        bool isType() const { return type() == rttr::type::get<T>(); }

    protected:
        const rttr::type m_type;    //!< The index in to the meta::list SocketTypes, which type this is!
        const SocketIndex m_index;  //!< The index of the slot at which this socket is installed in a LogicNode.
        LogicNode& m_parent;        //!< The parent node of of this socket.
    };

    //! The input socket base class for all input sockets of a node.
    class LogicSocketInputBase : public LogicSocketBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();

        friend class LogicSocketOutputBase;

        template<typename... Args>
        LogicSocketInputBase(Args&&... args)
            : LogicSocketBase(std::forward<Args>(args)...)
        {
        }

        ~LogicSocketInputBase();

        //! Cast to a logic socket of type `LogicSocketInput<T>*`.
        //! The cast fails at runtime if the data type `T` does not match!
        template<typename T>
        auto* castToType() const
        {
            EXECGRAPH_THROW_BADSOCKETCAST_IF(!this->template isType<T>(),
                                             "Casting socket index '{0}' with type index '{1}' into type"
                                             "'{2}' of node id: '{3}' which is wrong!",
                                             this->getIndex(),
                                             this->type(),
                                             demangle<T>(),
                                             this->parent().getId());

            return static_cast<const LogicSocketInput<T>*>(this);
        }

        //! Non-const overload.
        template<typename T>
        auto* castToType()
        {
            return const_cast<LogicSocketInput<T>*>(static_cast<LogicSocketInputBase const*>(this)->castToType<T>());
        }

        //! Set the Get-Link to an output socket.
        void setGetLink(LogicSocketOutputBase& outputSocket);

        //! Remove the Get-Link to an output socket.
        inline void removeGetLink()
        {
            removeGetLink<true>();
        }

        //! Check if the socket has a Get-Link to an output socket.
        inline bool hasGetLink() const { return m_getFrom != nullptr; }
        //! Get the output socket to which the Get-Link points.
        inline LogicSocketOutputBase* followGetLink() { return m_getFrom; }

        //! Get all sockets writing to this input socket.
        inline const auto& getWritingSockets() const { return m_writingParents; }
        //! Get the connection count of this input socket.
        inline IndexType getConnectionCount() const
        {
            return (hasGetLink() ? 1 : 0) + m_writingParents.size();
        }

    protected:
        //! Remove the Get-Link and optionally notify output.
        template<bool notifyOutput = true>
        void removeGetLink();

        //! Callback when the output socket has remove its Write-Link
        //! to this input socket.
        void onRemoveWritter(LogicSocketOutputBase& outputSocket);

        LogicSocketOutputBase* m_getFrom = nullptr;                   //!< The single Get-Link attached to this Socket.
        void const* m_data               = nullptr;                   //!< The pointer to the actual data of this input node.
        std::unordered_set<LogicSocketOutputBase*> m_writingParents;  //!< All parent output sockets which write to this input.
    };

    //! The input socket base class for all input/output sockets of a node.
    class LogicSocketOutputBase : public LogicSocketBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();
        friend class LogicSocketInputBase;

    protected:
        template<typename... Args>
        LogicSocketOutputBase(Args&&... args)
            : LogicSocketBase(std::forward<Args>(args)...)
        {
        }

        LogicSocketOutputBase(LogicSocketOutputBase&&) = default;
        LogicSocketOutputBase& operator=(LogicSocketOutputBase&&) = default;

    public:
        //! Cast to a logic socket of type `LogicSocketOutput`<T>*.
        //! The cast fails at runtime if the data type `T` does not match!
        template<typename T>
        auto* castToType() const noexcept(throwIfBadSocketCast)
        {
            EXECGRAPH_THROW_BADSOCKETCAST_IF(this->m_type != rttr::type::get<T>(),
                                             "Casting socket index '{0}' with type index '{1}' into "
                                             "'{2}' of node id: '{3}' which is wrong!",
                                             this->m_index,
                                             this->type(),
                                             demangle<T>(),
                                             this->m_parent.getId());

            return static_cast<const LogicSocketOutput<T>*>(this);
        }

        //! Non-const overload.
        template<typename T>
        auto* castToType()
        {
            return const_cast<LogicSocketOutput<T>*>(static_cast<LogicSocketOutputBase const*>(this)->castToType<T>());
        }

        void addWriteLink(LogicSocketInputBase& inputSocket);

        //! Remove Write-Link to the input socket `inputSocket`.
        void removeWriteLink(LogicSocketInputBase& inputSocket)
        {
            removeWriteLink<true>(inputSocket);
        }

        //! Remove all Write-Links to input sockets.
        void removeWriteLinks()
        {
            for(auto* inputSocket : m_writeTo)
            {
                inputSocket->onRemoveWritter(*this);
            }
            m_writeTo.clear();
        }

        const auto& getGetterSockets() { return m_getterChilds; }
        IndexType getConnectionCount() { return m_writeTo.size() + m_getterChilds.size(); }

    protected:
        template<bool notifyInput = true>
        void removeWriteLink(LogicSocketInputBase& inputSocket);

        //! Callback when input socket `child` has removed its Get-Link.
        inline void onRemoveGetter(LogicSocketInputBase& inputSocket)
        {
            m_getterChilds.erase(&inputSocket);
        }

        //! Write out value to all connected (Write-Link) input sockets.
        inline void executeWriteLinks()
        {
            for(auto* inputSocket : this->m_writeTo)
            {
                inputSocket->m_data = m_data;  // Set data pointer in input socket.
            }
        }

        //! Set the data reference of this socket.
        template<typename T>
        inline void setData(const T& data) { m_data = &data; }

    protected:
        //! All Write-Links attached to this Socket.
        std::vector<LogicSocketInputBase*> m_writeTo;
        //< All child sockets which have a Get-Link to this socket.
        std::unordered_set<LogicSocketInputBase*> m_getterChilds;

    private:
        //! The raw pointer to the actual data of this output socket.
        void const* const m_data = nullptr;
    };

    template<typename TData>
    class LogicSocketInput final : public LogicSocketInputBase
    {
    private:
        using LogicSocketInputBase::m_data;

    public:
        EXECGRAPH_DEFINE_TYPES();
        using Data = TData;

        template<typename... Args>
        LogicSocketInput(Args&&... args)
            : LogicSocketInputBase(rttr::type::get<Data>(),
                                   std::forward<Args>(args)...)
        {
        }

        //! Copy not allowed (since parent pointer)
        LogicSocketInput(const LogicSocketInput& other) = delete;
        LogicSocketInput& operator=(const LogicSocketInput& other) = delete;

        //! Move allowed
        LogicSocketInput(LogicSocketInput&& other) = default;
        LogicSocketInput& operator=(LogicSocketInput&& other) = default;

        //! If the socket has data.
        inline bool hasData() { return m_data != nullptr; }

        //! Get the data value of the socket. (follow Get-Link).
        //! If this input socket has not been connected, this results in an access violation!
        //! The graph checks that all input nodes ar connected when solving the execution order!
        inline const Data& data() const
        {
            EXECGRAPH_ASSERT(hasData(),
                             "Input socket index: '{0}' of node id: '{1}' not connected",
                             this->getIndex(),
                             this->parent().getId());
            return static_cast<const LogicSocketData<Data>*>(m_data)->data();
        }
    };

    template<typename TData>
    class LogicSocketOutput final : public LogicSocketOutputBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();
        using Data = TData;

        template<typename T,
                 typename... Args,
                 std::enable_if_t<!meta::is<T, LogicSocketOutput>::value, int> = 0>
        LogicSocketOutput(T&& initValue, Args&&... args)
            : LogicSocketOutputBase(rttr::type::get<Data>(),
                                    std::forward<Args>(args)...)
            , m_storage(std::forward<T>(initValue))
        {
            setData(data());
        }

        //! Copy not allowed (since parent pointer)
        LogicSocketOutput(const LogicSocketOutput& other) = delete;
        LogicSocketOutput& operator=(const LogicSocketOutput& other) = delete;

        //! Move allowed
        LogicSocketOutput(LogicSocketOutput&& other) = default;
        LogicSocketOutput& operator=(LogicSocketOutput&& other) = default;

        //! Set the data value of the socket.
        template<typename T>
        void setData(T&& value)
        {
            // Set the value
            data() = std::forward<T>(value);
            // Forward the value to all Write-Links
            this->executeWriteLinks();
        }

        //! Get the data value of the socket.
        inline const Data& data() const { return m_storage.data(); }
        //! Non-const overload.
        inline Data& data() { return m_storage.data(); }

    private:
        LogicSocketData<Data> m_storage;
    };

    //! Remove the Get-Link and optionally notify output.
    template<bool notifyOutput>
    void LogicSocketInputBase::removeGetLink()
    {
        if(hasGetLink())
        {
            // Set the data pointer to nullptr only if it points to
            // the getter at the moment, otherwise leave it as is.
            if(m_getFrom->m_data == m_data)
            {
                m_data = nullptr;
            }

            if(notifyOutput)
            {
                m_getFrom->onRemoveGetter(*this);
            }
            m_getFrom = nullptr;
        }
    }

    //! Remove Write-Link to input socket `inputSocket` and
    //! optionaly notify the input socket.
    template<bool notifyInput>
    void LogicSocketOutputBase::removeWriteLink(LogicSocketInputBase& inputSocket)
    {
        auto it = std::find(m_writeTo.begin(), m_writeTo.end(), &inputSocket);
        if(it != m_writeTo.end())
        {
            m_writeTo.erase(it);
        }
        if(notifyInput)
        {
            inputSocket.onRemoveWritter(*this);
        }
    }
}  // namespace executionGraph