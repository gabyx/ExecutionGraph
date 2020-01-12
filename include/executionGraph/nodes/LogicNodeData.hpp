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
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraph/nodes/LogicDataHandle.hpp"
#include "executionGraph/nodes/LogicNodeDataBase.hpp"
#include "executionGraph/nodes/LogicSocketBase.hpp"

namespace executionGraph
{
    template<typename TTraits, typename Parent>
    class LogicNodeDataConnections
    {
    public:
        using InputSocket            = typename TTraits::InputSocket;
        using OutputSocket           = typename TTraits::OutputSocket;
        using InputSocketConnection  = typename TTraits::InputSocketConnection;
        using OutputSocketConnection = typename TTraits::OutputSocketConnection;

    private:
        template<typename T>
        static constexpr bool isInputConnection = std::is_same_v<InputSocket, T>;
        template<typename T>
        static constexpr bool isOutputConnection = std::is_base_of_v<OutputSocket, T>;

        friend InputSocketConnection;
        friend OutputSocketConnection;

    public:
        explicit LogicNodeDataConnections(Parent& parent) noexcept
            : m_parent(&parent)
        {
        };

        ~LogicNodeDataConnections() noexcept
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

        LogicNodeDataConnections(const LogicNodeDataConnections&) = delete;
        LogicNodeDataConnections& operator=(const LogicNodeDataConnections&) = delete;

        LogicNodeDataConnections(LogicNodeDataConnections&& other)
        {
            // Connections are not  moved!
        }
        LogicNodeDataConnections& operator=(LogicNodeDataConnections&& other) = delete;

    public:
        void init(Parent& parent)
        {
            m_parent = &parent;
        }

        template<typename Socket,
                 EG_ENABLE_IF(isInputConnection<Socket> || isOutputConnection<Socket>)>
        void connect(Socket& socket) noexcept
        {
            if(!isConnected(socket))
            {
                onConnect(socket);
                socket.connections().onConnect(*this);
            }
        }

        template<typename Socket,
                 EG_ENABLE_IF(isInputConnection<Socket> || isOutputConnection<Socket>)>
        void disconnect(Socket& socket) noexcept
        {
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
    class LogicNodeDataRef;

    template<typename TData>
    class LogicNodeData final : public LogicNodeDataBase
    {
    public:
        using Data            = TData;
        using Base            = LogicNodeDataBase;
        using Connections     = ConnectionTraits<TData>::NodeDataConnections;
        using DataHandle      = LogicDataHandle<Data>;
        using DataHandleConst = LogicDataHandle<const Data>;

        static_assert(!std::is_const_v<Data> && !std::is_reference_v<Data>,
                      "Only non-const non-reference types allowed!");

        using InputSocket  = typename ConnectionTraits<Data>::InputSocket;
        using OutputSocket = typename ConnectionTraits<Data>::OutputSocket;

        using Reference = LogicNodeDataRef<Data>;

        friend Reference;

    public:
        template<typename... Args>
        LogicNodeData(NodeDataId id,
                      Args&&... args) noexcept
            : Base(rttr::type::get<Data>(), id)
            , m_data{std::forward<Args>(args)...}
            , m_connections(*this)
        {
        }

        ~LogicNodeData() noexcept
        {
            for(auto* ref : m_refs)
            {
                removeReference(*ref);
            }
        };

        //! Copy allowed
        LogicNodeData(const LogicNodeData& other)
            : m_connections(*this)
        {
            *this = other;
        };
        LogicNodeData& operator=(const LogicNodeData& other)
        {
            Base::operator=(other);
            m_data        = other.m_data;
        }

        //! Move allowed
        LogicNodeData(LogicNodeData&& other)
            : m_connections(*this)
        {
            *this = std::move(other);
        };
        LogicNodeData& operator=(LogicNodeData&& other)
        {
            Base::operator=(std::move(other));
            m_data        = std::move(other.m_data);
        }

    public:
        auto dataHandleConst() const noexcept
        {
            return DataHandleConst{m_data};
        }

        auto dataHandle() const noexcept
        {
            return dataHandleConst();
        }
        auto dataHandle() noexcept
        {
            return DataHandle{m_data};
        }

    public:
        void connect(LogicSocketInputBase& inputSocket) noexcept(false) override
        {
            m_connections.connect(inputSocket.castToType<Data, true>());
        }

        void connect(LogicSocketOutputBase& outputSocket) noexcept(false) override
        {
            m_connections.connect(outputSocket.castToType<Data, true>());
        }

        void disconnect(LogicSocketInputBase& inputSocket) noexcept override
        {
            m_connections.disconnect(inputSocket.castToType<Data, true>());
        }

        void disconnect(LogicSocketOutputBase& outputSocket) noexcept override
        {
            m_connections.disconnect(outputSocket.castToType<Data, true>());
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
    class LogicNodeDataRef final : public LogicNodeDataBase
    {
    public:
        using NodeData = LogicNodeData<TArgs...>;
        using Data     = typename NodeData::Data;

        using Base = LogicNodeDataBase;

        using DataHandle      = typename NodeData::DataHandle;
        using DataHandleConst = typename NodeData::DataHandleConst;

        static_assert(!std::is_const_v<Data> && !std::is_reference_v<Data>,
                      "Only non-const non-reference types allowed!");

        friend NodeData;

    public:
        template<typename... Args>
        LogicNodeDataRef(Args&&... args) noexcept
            : Base(rttr::type::get<Data>(), std::forward<Args>(args)...)
        {
        }

        ~LogicNodeDataRef() noexcept
        {
            removeReference();
        }

        DataHandleConst data() const noexcept
        {
            return m_node->data();
        }
        DataHandle data() noexcept
        {
            return m_node->data();
        }

    public:
        void setReference(const NodeData& node) noexcept
        {
            if(m_node)
            {
                m_node->onRemoveReference(*this);
            }
            m_node = &const_cast<NodeData&>(node);
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
        NodeData* m_node = nullptr;
    };
}  // namespace executionGraph