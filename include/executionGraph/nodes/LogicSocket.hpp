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

namespace executionGraph
{
    //! The socket base class for all input/output sockets of a node.
    class LogicSocketBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();

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
        SocketIndex getIndex() const { return m_index; }
        const rttr::type& type() const { return m_type; }

        const LogicNode& parent() const { return m_parent; }
        LogicNode& parent() { return m_parent; }

        template<typename T>
        bool isType() const { return type() == rttr::type::get<T>() }

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
        friend LogicSocketOutputBase;

        template<typename... Args>
        LogicSocketInputBase(Args&&... args)
            : LogicSocketBase(std::forward<Args>(args)...)
        {
        }

        ~LogicSocketInputBase()
        {
            EXECGRAPH_LOG_TRACE("Destructor: LogicSocketInputBase: index: '{0}', parent: '{1}'",
                                this->getIndex(),
                                fmt::ptr(&this->parent()));

            // Reset Get-Link
            removeGetLink();

            // Reset all Write-Links
            for(auto* outputSocket : m_writingParents)
            {
                outputSocket->template removeWriteLink<false>(*this);
            }
        }

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

            return static_cast<LogicSocketInput<T> const*>(this);
        }

        //! Non-const overload.
        template<typename T>
        auto* castToType()
        {
            return const_cast<LogicSocketInput<T>*>(static_cast<LogicSocketInputBase const*>(this)->castToType<T>());
        }

        //! Set the Get-Link to an output socket.
        void setGetLink(LogicSocketOutputBase& outputSocket)
        {
            EXECGRAPH_THROW_TYPE_IF(outputSocket.parent().getId() == this->parent().getId(),
                                    NodeConnectionException,
                                    "No Get-Link connection to our output slot! (node id: '{0}')",
                                    this->parent().getId());

            EXECGRAPH_THROW_TYPE_IF(this->type() != outputSocket.type(),
                                    NodeConnectionException,
                                    "Output socket index: '{0}' of node id: '{1}' has not the same type as "
                                    "input socket index: '{2}' of node id: '{3}'!",
                                    outputSocket.getIndex(),
                                    outputSocket.parent().getId(),
                                    this->getIndex(),
                                    this->parent().getId());

            EXECGRAPH_THROW_TYPE_IF(m_writingParents.find(&outputSocket) != m_writingParents.end(),
                                    NodeConnectionException,
                                    "Cannot add Get-Link from input socket index: '{0}' of node id: '{1}' to "
                                    "output socket index '{2}' of node id: '{3}' because output already has a "
                                    "Write-Link to this input!",
                                    this->getIndex(),
                                    this->parent().getId(),
                                    outputSocket.getIndex(),
                                    outputSocket.parent().getId());

            // Remove Get-Link (if existing)
            removeGetLink();

            m_getFrom = &outputSocket;
            m_data    = outputSocket.m_data;  // Set data pointer of this input socket.
            outputSocket.m_getterChilds.emplace(this);
        }

        //! Remove the Get-Link to an output socket.
        void removeGetLink()
        {
            removeGetLink<true>();
        }

        //! Check if the socket has a Get-Link to an output socket.
        bool hasGetLink() const { return m_getFrom != nullptr; }
        //! Get the output socket to which the Get-Link points.
        LogicSocketOutputBase* followGetLink() { return m_getFrom; }

        //! Get all sockets writing to this input socket.
        const auto& getWritingSockets() const { return m_writingParents; }
        //! Get the connection count of this input socket.
        IndexType getConnectionCount() const
        {
            return (hasGetLink() ? 1 : 0) + m_writingParents.size();
        }

    protected:
        //! Remove the Get-Link and optionally notify output.
        template<bool notifyOutput = true>
        void removeGetLink()
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

        //! Callback when the output socket has remove its Write-Link
        //! to this input socket.
        void onRemoveWritter(LogicSocketOutputBase& outputSocket)
        {
            m_writingParents.erase(&outputSocket);

            // If the data pointer points to this output socket try
            // to reroute to the Get-Link if possible.
            if(m_data == outputSocket.m_data)
            {
                m_data = hasGetLink() ? m_getFrom->m_data : nullptr;
            }
        }

        LogicSocketOutputBase* m_getFrom = nullptr;                   //!< The single Get-Link attached to this Socket.
        void const* m_data               = nullptr;                   //!< The pointer to the actual data of this input node.
        std::unordered_set<LogicSocketOutputBase*> m_writingParents;  //!< All parent output sockets which write to this input.
    };

    //! The input socket base class for all input/output sockets of a node.
    class LogicSocketOutputBase : public LogicSocketBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();
        friend LogicSocketInputBase;

    protected:
        template<typename T, typename... Args>
        LogicSocketOutputBase(const T& data, Args&&... args)
            : LogicSocketBase(std::forward<Args>(args)...)
            , m_data(static_cast<const void*>(&data))
        {
        }

        ~LogicSocketOutputBase()
        {
            EXECGRAPH_LOG_TRACE("Destructor: LogicSocketOutputBase: index: '{0}', parent: '{1}'",
                                this->getIndex(),
                                fmt::ptr(&this->parent()));

            removeWriteLinks();

            // Reset data address in all input sockets.
            for(auto* inSocket : m_getterChilds)
            {
                inSocket->template removeGetLink<false>();
            }
        }

    public:
        //! Cast to a logic socket of type `LogicSocketOutput`<T>*.
        //! The cast fails at runtime if the data type `T` does not match!
        template<typename T>
        auto* castToType() const noexcept(throwIfBadSocketCast)
        {
            EXECGRAPH_THROW_BADSOCKETCAST_IF((this->m_type != meta::find_index<SocketTypes, T>::value),
                                             "Casting socket index '{0}' with type index '{1}' into "
                                             "'{2}' of node id: '{3}' which is wrong!",
                                             this->m_index,
                                             this->type(),
                                             demangle<T>(),
                                             this->m_parent.getId());

            return static_cast<LogicSocketOutput<T> const*>(this);
        }

        //! Non-const overload.
        template<typename T>
        auto* castToType()
        {
            return const_cast<LogicSocketOutput<T>*>(static_cast<LogicSocketOutputBase const*>(this)->castToType<T>());
        }

        void addWriteLink(LogicSocketInputBase& inputSocket)
        {
            EXECGRAPH_THROW_TYPE_IF(inputSocket.parent().getId() == this->parent().getId(),
                                    NodeConnectionException,
                                    "No Write-Link connection to our input slot! (node id: '{0}')",
                                    this->parent().getId());

            EXECGRAPH_THROW_TYPE_IF(this->type() != inputSocket.type(),
                                    NodeConnectionException,
                                    "Output socket index: '{1}' of node id: '{2}' "
                                    "has not the same type as input socket index '{3}'"
                                    "of node id: '{4}'",
                                    this->getIndex(),
                                    this->parent().getId(),
                                    inputSocket.getIndex(),
                                    inputSocket.parent().getId());

            EXECGRAPH_THROW_TYPE_IF(m_getterChilds.find(&inputSocket) != m_getterChilds.end(),
                                    NodeConnectionException,
                                    "Cannot add Write-Link from output socket index: '{0}' of node id: '{1}' to "
                                    "input socket index '{2} of node id: '{3}' because input "
                                    "already has a Get-Link to this output!",
                                    this->getIndex(),
                                    this->parent().getId(),
                                    inputSocket.getIndex(),
                                    inputSocket.parent().getId());

            if(std::find(m_writeTo.begin(), m_writeTo.end(), &inputSocket) == m_writeTo.end())
            {
                m_writeTo.push_back(&inputSocket);
                inputSocket.m_writingParents.emplace(this);
            }
        }

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
        //! Remove Write-Link to input socket `inputSocket` and optionaly notify the input socket.
        template<bool notifyInput = true>
        void removeWriteLink(LogicSocketInputBase& inputSocket)
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
        //! Callback when input socket `child` has removed its Get-Link.
        void onRemoveGetter(LogicSocketInputBase& inputSocket)
        {
            m_getterChilds.erase(&inputSocket);
        }

        //! Write out value to all connected (Write-Link) input sockets.
        void executeWriteLinks()
        {
            for(auto* inputSocket : this->m_writeTo)
            {
                inputSocket->m_data = m_data;  // Set data pointer in input socket.
            }
        }

    protected:
        //! All Write-Links attached to this Socket.
        std::vector<LogicSocketInputBase*> m_writeTo;
        //< All child sockets which have a Get-Link to this socket.
        std::unordered_set<LogicSocketInputBase*> m_getterChilds;

    private:
        void const* const m_data = nullptr;  //!< The raw pointer to the actual data of this output socket.
    };

    template<typename TData, typename TDataStorage>
    class LogicSocketOutput;

    template<typename TData>
    class LogicSocketInput final : public LogicSocketInputBase
    {
    private:
        using LogicSocketInputBase::m_data;

    public:
        EXECGRAPH_DEFINE_TYPES();
        using DataType = TData;

        template<typename... Args>
        LogicSocketInput(Args&&... args)
            : LogicSocketInputBase(rttr::type::get<DataType>(),
                                   std::forward<Args>(args)...)
        {
        }

        //! Copy not allowed (since parent pointer)
        LogicSocketInput(LogicSocketInput& other) = delete;
        LogicSocketInput& operator=(LogicSocketInput& other) = delete;

        //! Move allowed
        LogicSocketInput& operator=(LogicSocketInput&& other) = default;
        LogicSocketInput(LogicSocketInput&& other)            = default;

        //! If the socket has data.
        bool hasData() { return m_data != nullptr; }

        //! Get the data value of the socket. (follow Get-Link).
        //! If this input socket has not been connected, this results in an access violation!
        //! The graph checks that all input nodes ar connected when solving the execution order!
        const DataType& data() const
        {
            EXECGRAPH_ASSERT(hasData(),
                             "Input socket index: '{0}' of node id: '{1}' not connected",
                             this->getIndex(),
                             this->parent().getId());
            return static_cast<const LogicSocketOutput<DataType>*>(m_data)->data();
        }
        //! Non-const overload.
        const DataType& data()
        {
            return const_cast<const DataType&>(static_cast<const LogicSocketInput*>(this)->data());
        }
    };

    //! Data wrapper for the output socket.
    template<typename TData>
    class LogicSocketData
    {
    public:
        using DataType = TData;

    public:
        template<typename T>
        LogicSocketData(T&& value)
            : m_data(std::forward<T>(value)) {}

        DataType& data() { return m_data; }
        const DataType& data() const { return m_data; }

    private:
        DataType m_data;  //!< The data.
    };

    template<typename TData,
             typename TDataStorage = LogicSocketData<TData>>
    class LogicSocketOutput final : public LogicSocketOutputBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();
        using DataStorage = TDataStorage;
        using DataType    = TData;

        template<typename T, typename... Args>
        LogicSocketOutput(T&& initValue, Args&&... args)
            : LogicSocketOutputBase(this,
                                    rttr::type::get<DataType>(),
                                    std::forward<Args>(args)...)
            , m_storage(std::forward<T>(initValue))
        {
        }

        //! Copy not allowed (since parent pointer)
        LogicSocketOutput(LogicSocketOutput& other) = delete;
        LogicSocketOutput& operator=(LogicSocketOutput& other) = delete;

        //! Move allowed
        LogicSocketOutput& operator=(LogicSocketOutput&& other) = default;
        LogicSocketOutput(LogicSocketOutput&& other)            = default;

        //! Set the data value of the socket.
        template<typename T>
        void setData(T&& value)
        {
            // Set the value
            DataStorage::m_data = std::forward<T>(value);
            // Forward the value to all Write-Links
            this->executeWriteLinks();
        }

        //! Get the data value of the socket.
        const DataType& data() const { return m_storage.data(); }
        //! Non-const overload.
        DataType& data() { return m_storage.data(); }

    private:
        DataStorage m_storage;
    };

}  // namespace executionGraph