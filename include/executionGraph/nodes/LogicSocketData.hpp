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
    template<typename TTraits>
    class LogicSocketDataConnectionsBase
    {
    public:
        using Data         = typename TTraits::Data;
        using InputSocket  = typename TTraits::InputSocket;
        using OutputSocket = typename TTraits::OutputSocket;

        using InputSocketConnections  = typename TTraits::InputSocketConnections;
        using OutputSocketConnections = typename TTraits::OutputSocketConnections;

        using SocketDataBase    = typename TTraits::SocketDataBase;
        using ISocketDataAccess = typename TTraits::ISocketDataAccess;

        friend InputSocketConnections;
        friend OutputSocketConnections;

    protected:
        LogicSocketDataConnectionsBase(SocketDataBase& parent)
            : m_parent(parent)
        {
        }

        LogicSocketDataConnectionsBase(const LogicSocketDataConnectionsBase&)  = delete;
        LogicSocketDataConnectionsBase(LogicSocketDataConnectionsBase&& other) = default;

    private:
        virtual void onConnect(const OutputSocket& outputSocket) noexcept = 0;
        virtual void onConnect(const InputSocket& inputSocket) noexcept   = 0;

        virtual void onDisconnect(const OutputSocket& outputSocket) noexcept = 0;
        virtual void onDisconnect(const InputSocket& inputSocket) noexcept   = 0;

        virtual ISocketDataAccess& dataAccess() noexcept = 0;
        const ISocketDataAccess& dataAccess() const noexcept
        {
            return dataAccessConst();
        }
        const ISocketDataAccess& dataAccessConst() const noexcept
        {
            return const_cast<LogicSocketDataConnectionsBase&>(*this)
                .dataAccess();
        }

    public:
        SocketDataBase& parent() noexcept
        {
            return m_parent;
        }

        const SocketDataBase& parent() const noexcept
        {
            return const_cast<LogicSocketDataConnectionsBase&>(*this).parent();
        }

    protected:
        struct Forwarder
        {
            template<typename Socket>
            static void onDisconnect(Socket& socket)
            {
                socket.connections().onDisconnect();
            }

            template<typename Socket>
            static void onConnect(Socket& socket, LogicSocketDataConnectionsBase& self)
            {
                socket.connections().onConnect(self);
            }
        };

    private:
        SocketDataBase& m_parent;
    };

    template<typename TTraits, typename Parent>
    class LogicSocketDataConnections final : public TTraits::SocketDataConnectionsBase
    {
    public:
        using Base                    = typename TTraits::SocketDataConnectionsBase;
        using InputSocket             = typename TTraits::InputSocket;
        using OutputSocket            = typename TTraits::OutputSocket;
        using InputSocketConnections  = typename TTraits::InputSocketConnections;
        using OutputSocketConnections = typename TTraits::OutputSocketConnections;
        using ISocketDataAccess       = typename TTraits::ISocketDataAccess;

    private:
        template<typename T>
        static constexpr bool isInputConnection = std::is_same_v<InputSocket, T>;
        template<typename T>
        static constexpr bool isOutputConnection = std::is_same_v<OutputSocket, T>;

    public:
        explicit LogicSocketDataConnections(Parent& parent) noexcept
            : Base(parent)
            , m_parent(parent){};

        ~LogicSocketDataConnections() noexcept
        {
            for(auto* socket : m_inputs)
            {
                Base::Forwarder::onDisconnect(*socket);
            }
            for(auto* socket : m_outputs)
            {
                Base::Forwarder::onDisconnect(*socket);
            }
        }

        LogicSocketDataConnections(const LogicSocketDataConnections&) = delete;
        LogicSocketDataConnections& operator=(const LogicSocketDataConnections&) = delete;

        LogicSocketDataConnections(LogicSocketDataConnections&& other)
            : Base(std::move(other))
            , m_parent(other.m_parent)
        {
            m_inputs  = std::move(other.m_inputs);
            m_outputs = std::move(other.m_outputs);

            other.m_inputs.clear();
            other.m_inputs.clear();
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
                Base::Forwarder::onConnect(socket, *this);
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
                Base::Forwarder::onDisconnect(socket);
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

    public:
        Parent& parent()
        {
            return m_parent;
        }
        const Parent& parent() const
        {
            return const_cast<LogicSocketDataConnections&>(*this).parent();
        }

    private:
        void onConnect(const InputSocket& socket) noexcept override
        {
            EG_VERIFY(addGetLink(socket), "Not connected!");
        }

        void onConnect(const OutputSocket& socket) noexcept override
        {
            EG_VERIFY(addWriteLink(socket), "Not connected!");
        }

        void onDisconnect(const InputSocket& socket) noexcept override
        {
            EG_VERIFY(removeGetLink(socket), "Not disconnected!");
        }

        void onDisconnect(const OutputSocket& socket) noexcept override
        {
            EG_VERIFY(removeWriteLink(socket), "Not disconnected!");
        }

    public:
        ISocketDataAccess& dataAccess() noexcept override
        {
            return parent().dataAccess();
        };

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
        //! The parent of this class.
        Parent& m_parent;
        //! All inputs reading this data node.
        std::unordered_set<InputSocket*> m_inputs;
        //! All outputs writting to this data node.
        std::unordered_set<OutputSocket*> m_outputs;
    };

    template<typename...>
    class LogicSocketDataRef;

    /* ---------------------------------------------------------------------------------------*/
    /*!
         A basic Implementation of the data node. 
         No locking mechanism is implemented here.
         This node owns the data which is constructed in place in the constructor

        @date Wed Apr 01 2020
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TData>
    class LogicSocketData final : public LogicSocketDataBase,
                                  public ILogicSocketDataAccess<TData>
    {
    public:
        using Data              = TData;
        using Base              = LogicSocketDataBase;
        using ISocketDataAccess = ILogicSocketDataAccess<TData>;

        using Connections = ConnectionTraits<TData>::SocketDataConnections;

        EG_STATIC_ASSERT(!std::is_const_v<Data> && !std::is_reference_v<Data>,
                         "Only non-const non-reference types allowed!");

        using InputSocket  = typename ConnectionTraits<Data>::InputSocket;
        using OutputSocket = typename ConnectionTraits<Data>::OutputSocket;

        using Reference = LogicSocketDataRef<Data>;
        friend Reference;

        using DataHandle      = typename ISocketDataAccess::DataHandle;
        using DataHandleConst = typename ISocketDataAccess::DataHandleConst;

    private:
        //! DataHandle which is convertible to LogicDataHandle.
        template<typename T>
        struct InternalDataHandle
        {
            T& m_data = nullptr;
            T* data() { return &m_data; }
        };

    public:
        template<typename... Args>
        LogicSocketData(SocketDataId id,
                        Args&&... args) noexcept
            : Base(rttr::type::get<Data>(), id, *this)
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
        ISocketDataAccess& dataAccess() { return *this; }
        const ISocketDataAccess& dataAccess() const
        {
            const_cast<LogicSocketData&>(*this).dataAccess();
        }
        const ISocketDataAccess& dataAccessConst() const { return *this; }

    public:
        DataHandleConst dataHandleConst() const noexcept override
        {
            return DataHandleConst(InternalDataHandle<const Data>{m_data});
        }

        DataHandleConst dataHandle() const noexcept override
        {
            return dataHandleConst();
        }

        DataHandle dataHandle() noexcept override
        {
            return DataHandle(InternalDataHandle<Data>{m_data});
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

        template<typename... Args>
        LogicSocketDataRef(SocketData& socketData, Args&&... args) noexcept
            : Base(rttr::type::get<Data>(), std::forward<Args>(args)..., socketData)
            , m_socketData(&socketData)
        {
        }

        ~LogicSocketDataRef() noexcept
        {
            setReference(nullptr);
        }

        DataHandleConst dataHandle() const noexcept
        {
            EG_ASSERT(m_socketData,
                      "Reference not set at data socket ref with id: '{0}'",
                      id());
            return m_socketData->dataHandle();
        }

        DataHandle dataHandle() noexcept
        {
            EG_ASSERT(m_socketData,
                      "Reference not set at data socket ref with id: '{0}'",
                      id());
            return m_socketData->dataHandle();
        }

        DataHandleConst dataHandleConst() noexcept
        {
            EG_ASSERT(m_socketData,
                      "Reference not set at data socket ref with id: '{0}'",
                      id());
            return m_socketData->dataHandleConst();
        }


    public:
        virtual void connect(LogicSocketInputBase& inputSocket) noexcept(false) override
        {
            if(m_socketData)
            {
                m_socketData->connect(inputSocket);
            }
        }
        virtual void connect(LogicSocketOutputBase& outputSocket) noexcept(false) override
        {
            if(m_socketData)
            {
                m_socketData->connect(outputSocket);
            }
        }
        virtual void disconnect(LogicSocketInputBase& inputSocket) noexcept(false) override
        {
            if(m_socketData)
            {
                m_socketData->disconnect(inputSocket);
            }
        }
        virtual void disconnect(LogicSocketOutputBase& outputSocket) noexcept(false) override
        {
            if(m_socketData)
            {
                m_socketData->disconnect(outputSocket);
            }
        }

    public:
        void setReference(const SocketData& node) noexcept
        {
            if(m_socketData)
            {
                m_socketData->onRemoveReference(*this);
            }
            m_socketData = &const_cast<SocketData&>(node);
            m_socketData->onSetReference(*this);
        }

    private:
        void setReference(std::nullptr_t) noexcept
        {
            if(m_socketData)
            {
                m_socketData->onRemoveReference(*this);
            }
            m_socketData = nullptr;
        }

        void onRemoveReference() noexcept
        {
            m_socketData = nullptr;
        }

    private:
        SocketData* m_socketData = nullptr;
    };
}  // namespace executionGraph