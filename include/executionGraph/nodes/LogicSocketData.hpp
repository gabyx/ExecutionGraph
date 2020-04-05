// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Aug 21 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <type_traits>
#include <unordered_set>
#include <meta/meta.hpp>
#include <rttr/type>
#include "executionGraph/common/Assert.hpp"
#include "executionGraph/common/StaticAssert.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraph/nodes/LogicDataHandle.hpp"
#include "executionGraph/nodes/LogicSocket.hpp"
#include "executionGraph/nodes/LogicSocketDataBase.hpp"

namespace executionGraph
{
    template<typename TTraits, typename Parent>
    class LogicSocketDataConnections
    {
    public:
        using InputSocket             = typename TTraits::InputSocket;
        using OutputSocket            = typename TTraits::OutputSocket;
        using InputSocketConnections  = typename TTraits::InputSocketConnections;
        using OutputSocketConnections = typename TTraits::OutputSocketConnections;

    private:
        template<typename T>
        static constexpr bool isInputConnection = std::is_same_v<InputSocket, T>;
        template<typename T>
        static constexpr bool isOutputConnection = std::is_same_v<OutputSocket, T>;

        friend InputSocketConnections;
        friend OutputSocketConnections;

    public:
        explicit LogicSocketDataConnections(Parent& parent) noexcept
            : m_parent(&parent){};

        ~LogicSocketDataConnections() noexcept
        {
            for(auto* socket : m_inputs)
            {
                socket->connections().onDisconnect();
            }
            for(auto* socket : m_outputs)
            {
                socket->connections().onDisconnect();
            }
        }

        LogicSocketDataConnections(const LogicSocketDataConnections&) = delete;
        LogicSocketDataConnections& operator=(const LogicSocketDataConnections&) = delete;

        LogicSocketDataConnections(LogicSocketDataConnections&& other)
        {
            *this = std::move(other);
        }

        LogicSocketDataConnections& operator=(LogicSocketDataConnections&& other)
        {
            m_inputs  = std::move(other.m_inputs);
            m_outputs = std::move(other.m_outputs);

            other.m_inputs.clear();
            other.m_inputs.clear();
            return *this;
        };

    public:
        void init(Parent& parent)
        {
            m_parent = &parent;
        }

        template<typename Socket>
        void connect(Socket& socket) noexcept
        {
            EG_STATIC_ASSERT(!std::is_const_v<Socket>, "Socket type is const");
            EG_STATIC_ASSERT(isInputConnection<Socket> || isOutputConnection<Socket>,
                             "Socket doesn't match this data socket");

            if(!isConnected(socket))
            {
                onConnect(socket);
                socket.connections().onConnect(*this);
            }
        }

        template<typename Socket>
        void disconnect(Socket& socket) noexcept
        {
            EG_STATIC_ASSERT(!std::is_const_v<Socket>, "Socket type is const");
            EG_STATIC_ASSERT(isInputConnection<Socket> || isOutputConnection<Socket>,
                             "Socket doesn't match this data socket");

            if(isConnected(socket))
            {
                onDisconnect(socket);
                socket.connections().onDisconnect();
            }
        }

        bool isConnected(const InputSocket& socket) noexcept
        {
            return m_inputs.find(const_cast<InputSocket*>(&socket)) != m_inputs.end();
        }

        bool isConnected(const OutputSocket& socket) noexcept
        {
            return m_outputs.find(const_cast<OutputSocket*>(&socket)) != m_outputs.end();
        }

    protected:
        Parent& parent() { return *m_parent; }
        const Parent& parent() const { return *m_parent; }

        void onConnect(const InputSocket& socket) noexcept
        {
            EG_VERIFY(addGetLink(socket), "Not connected!");
        }

        void onConnect(const OutputSocket& socket) noexcept
        {
            EG_VERIFY(addWriteLink(socket), "Not connected!");
        }

        void onDisconnect(const InputSocket& socket) noexcept
        {
            EG_VERIFY(removeGetLink(socket), "Not disconnected!");
        }

        void onDisconnect(const OutputSocket& socket) noexcept
        {
            EG_VERIFY(removeWriteLink(socket), "Not disconnected!");
        }

    private:
        bool addWriteLink(const OutputSocket& output) noexcept
        {
            return m_outputs.emplace(&const_cast<OutputSocket&>(output)).second;
        }
        bool removeWriteLink(const OutputSocket& output) noexcept
        {
            return m_outputs.erase(&const_cast<OutputSocket&>(output)) > 0;
        }

        bool addGetLink(const InputSocket& input) noexcept
        {
            return m_inputs.emplace(&const_cast<InputSocket&>(input)).second;
        }
        bool removeGetLink(const InputSocket& input) noexcept
        {
            return m_inputs.erase(&const_cast<InputSocket&>(input)) > 0;
        }

    public:
        const auto& outputs() const noexcept { return m_outputs; }
        const auto& inputs() const noexcept { return m_inputs; }

    private:
        //! All inputs reading this data node.
        std::unordered_set<InputSocket*> m_inputs;
        //! All outputs writting to this data node.
        std::unordered_set<OutputSocket*> m_outputs;
        //! The parent of this class.
        Parent* m_parent = nullptr;
    };

    template<typename...>
    class LogicSocketDataRef;

    /* ---------------------------------------------------------------------------------------*/
    /*!
         Basic Implementation of the data node. 
         No locking mechanism is implemented here.
         This node owns the data which is constructed in place in the constructor

        @date Wed Apr 01 2020
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TData>
    class LogicSocketData final : public LogicSocketDataBase
    {
    public:
        using Data            = TData;
        using Base            = LogicSocketDataBase;
        using Connections     = ConnectionTraits<TData>::SocketDataConnections;
        using DataHandle      = LogicDataHandle<Data>;
        using DataHandleConst = LogicDataHandle<const Data>;

        EG_STATIC_ASSERT(!std::is_const_v<Data> && !std::is_reference_v<Data>,
                         "Only non-const non-reference types allowed!");

        using InputSocket  = typename ConnectionTraits<Data>::InputSocket;
        using OutputSocket = typename ConnectionTraits<Data>::OutputSocket;

        using Reference = LogicSocketDataRef<Data>;

        friend Reference;

    public:
        template<typename... Args>
        LogicSocketData(SocketDataId id,
                        Args&&... args) noexcept
            : Base(rttr::type::get<Data>(), id)
            , m_data{std::forward<Args>(args)...}
            , m_connections(*this)
        {
        }

        ~LogicSocketData() noexcept
        {
            for(auto* ref : m_refs)
            {
                removeReference(*ref);
            }
        };

        //! Move allowed
        LogicSocketData(LogicSocketData&& other)
            : Base(std::move(other))
            , m_data{std::move(other.m_data)}
            , m_connections{std::move(other.m_connections)} {};

        LogicSocketData& operator=(LogicSocketData&& other)
        {
            Base::operator=(std::move(other));
            m_data        = std::move(other.m_data);
            m_connections = std::move(other.m_connections);
        }

    public:
        DataHandleConst dataHandleConst() const noexcept
        {
            return DataHandleConst{m_data};
        }

        DataHandleConst dataHandle() const noexcept
        {
            return dataHandleConst();
        }

        DataHandle dataHandle() noexcept
        {
            return DataHandle{m_data};
        }

    public:
        void connect(LogicSocketInputBase& inputSocket) noexcept(false) override
        {
            connectImpl(inputSocket);
        }

        void connect(LogicSocketOutputBase& outputSocket) noexcept(false) override
        {
            connectImpl(outputSocket);
        }

        void disconnect(LogicSocketInputBase& inputSocket) noexcept(false) override
        {
            connectImpl<true>(inputSocket);
        }

        void disconnect(LogicSocketOutputBase& outputSocket) noexcept(false) override
        {
            connectImpl<true>(outputSocket);
        }

        template<typename Socket,
                 EG_ENABLE_IF(!std::is_same_v<Socket, LogicSocketInputBase> &&
                              !std::is_same_v<Socket, LogicSocketOutputBase>)>
        void connect(Socket& socket) noexcept
        {
            m_connections.connect(socket);
        }

        template<typename Socket,
                 EG_ENABLE_IF(!std::is_same_v<Socket, LogicSocketInputBase> &&
                              !std::is_same_v<Socket, LogicSocketOutputBase>)>
        void disconnect(Socket& socket) noexcept
        {
            m_connections.connect(socket);
        }

    private:
        template<bool disconnect = false, typename Socket>
        void connectImpl(Socket& socket) noexcept(false)
        {
            try
            {
                auto& s = socket.template castToType<Data, true>();  //  throws ...

                if constexpr(disconnect)
                {
                    m_connections.disconnect(s);
                }
                else
                {
                    m_connections.connect(s);
                }
            }
            catch(BadSocketCastException&)
            {
                EG_THROW_TYPE(NodeConnectionException,
                              "{0} failed from socket index '{1}' [{2}] at node id '{3}'"
                              "to data node id '{4}' [{5}]",
                              socket.parent().id(),
                              socket.index(),
                              socket.type().get_name(),
                              id(),
                              type().get_name(),
                              disconnect ? "Connection"
                                         : "Disconnection");
            }
        }

    public:
        auto& connections() noexcept
        {
            return m_connections;
        }

        const auto& connections() const noexcept
        {
            return m_connections;
        }

    private:
        bool removeReference(const Reference& ref) noexcept
        {
            const_cast<Reference&>(ref).onRemoveReference();
            return m_refs.erase(&ref) > 0;
        }

        bool onSetReference(const Reference& ref) noexcept
        {
            return m_refs.emplace(&ref).second;
        }

        bool onRemoveReference(const Reference& ref) noexcept
        {
            return m_refs.erase(&ref) > 0;
        }

    private:
        //! The underlying data storage.
        Data m_data;
        //! All data nodes refs referencing this data node.
        std::unordered_set<const Reference*> m_refs;
        //! The connections.
        Connections m_connections;
    };

    template<typename... TArgs>
    class LogicSocketDataRef final : public LogicSocketDataBase
    {
    public:
        using SocketData = LogicSocketData<TArgs...>;
        using Data       = typename SocketData::Data;

        using Base = LogicSocketDataBase;

        using DataHandle      = typename SocketData::DataHandle;
        using DataHandleConst = typename SocketData::DataHandleConst;

        EG_STATIC_ASSERT(!std::is_const_v<Data> && !std::is_reference_v<Data>,
                         "Only non-const non-reference types allowed!");

        friend SocketData;

    public:
        template<typename... Args>
        LogicSocketDataRef(Args&&... args) noexcept
            : Base(rttr::type::get<Data>(), std::forward<Args>(args)...)
        {
        }

        ~LogicSocketDataRef() noexcept
        {
            removeReference();
        }

        DataHandleConst dataHandle() const noexcept
        {
            return m_node->dataHandle();
        }
        DataHandle dataHandle() noexcept
        {
            return m_node->dataHandle();
        }

    public:
        virtual void connect(LogicSocketInputBase& inputSocket) noexcept(false) override
        {
            if(m_node)
            {
                m_node->connect(inputSocket);
            }
        }
        virtual void connect(LogicSocketOutputBase& outputSocket) noexcept(false) override
        {
            if(m_node)
            {
                m_node->connect(outputSocket);
            }
        }
        virtual void disconnect(LogicSocketInputBase& inputSocket) noexcept(false) override
        {
            if(m_node)
            {
                m_node->disconnect(inputSocket);
            }
        }
        virtual void disconnect(LogicSocketOutputBase& outputSocket) noexcept(false) override
        {
            if(m_node)
            {
                m_node->disconnect(outputSocket);
            }
        }

    public:
        void setReference(const SocketData& node) noexcept
        {
            if(m_node)
            {
                m_node->onRemoveReference(*this);
            }
            m_node = &const_cast<SocketData&>(node);
            m_node->onSetReference(*this);
        }

    private:
        void removeReference() noexcept
        {
            if(m_node)
            {
                m_node->onRemoveReference(*this);
            }
            m_node = nullptr;
        }

        void onRemoveReference() noexcept
        {
            m_node = nullptr;
        }

    private:
        SocketData* m_node = nullptr;
    };
}  // namespace executionGraph