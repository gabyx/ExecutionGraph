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
    template<typename TTraits, typename Derived>
    class LogicNodeDataConnections
    {
    public:
        using InputSocket             = typename TTraits::InputSocket;
        using OutputSocket            = typename TTraits::OutputSocket;
        using InputSocketConnections  = typename TTraits::InputSocketConnections;
        using OutputSocketConnections = typename TTraits::OutputSocketConnections;

    private:
        template<typename T>
        static constexpr bool isInputConnection = std::is_base_of_v<InputSocketConnections, T>;
        template<typename T>
        static constexpr bool isOutputConnection = std::is_base_of_v<OutputSocketConnections, T>;

        static_assert(isInputConnection<InputSocket> && isOutputConnection<OutputSocket>,
                      "SocketConnections needs to be a base of InputSocket and OutputSocket");

        friend InputSocketConnections;
        friend OutputSocketConnections;

    protected:
        LogicNodeDataConnections() noexcept = default;

        ~LogicNodeDataConnections() noexcept
        {
            for(auto* socket : m_inputs)
            {
                static_cast<InputSocketConnections*>(socket)->onDisconnect();
            }
            for(auto* socket : m_outputs)
            {
                static_cast<OutputSocketConnections*>(socket)->onDisconnect();
            }
        }

    public:
        template<typename Socket>
        void connect(Socket& socket) noexcept
        {
            if(!isConnected(socket))
            {
                onConnect(socket);
                if constexpr(isInputConnection<Socket>)
                {
                    static_cast<InputSocketConnections&>(socket).onConnect(static_cast<Derived&>(*this));
                }
                else
                {
                    static_cast<OutputSocketConnections&>(socket).onConnect(static_cast<Derived&>(*this));
                }
            }
        }

        template<typename Socket>
        void disconnect(Socket& socket) noexcept
        {
            if(isConnected(socket))
            {
                onDisconnect(socket);
                if constexpr(isInputConnection<Socket>)
                {
                    static_cast<InputSocketConnections&>(socket).onDisconnect();
                }
                else
                {
                    static_cast<OutputSocketConnections&>(socket).onDisconnect();
                }
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
            EG_VERIFY(removeGetLink(socket), "Not connected!");
        }

        void onDisconnect(const OutputSocket& socket) noexcept
        {
            EG_VERIFY(removeWriteLink(socket), "Not connected!");
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
        std::unordered_set<InputSocket*> m_inputs;    //! All inputs reading this data node.
        std::unordered_set<OutputSocket*> m_outputs;  //! All outputs writting to this data node.
    };

    template<typename...>
    class LogicNodeDataRef;

    template<typename TData>
    class LogicNodeData final : public LogicNodeDataBase,
                                public ConnectionTraits<TData>::NodeDataConnections

    {
    public:
        using Data            = TData;
        using Base            = LogicNodeDataBase;
        using ConnectionBase  = ConnectionTraits<TData>::NodeDataConnections;
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
        {
        }

        ~LogicNodeData() noexcept
        {
            for(auto* ref : m_refs)
            {
                removeReference(*ref);
            }
        };

    public:
        auto cdata() const noexcept
        {
            return DataHandleConst{m_data};
        }

        auto data() const noexcept
        {
            return cdata();
        }
        auto data() noexcept
        {
            return DataHandle{m_data};
        }

    public:
        void connect(LogicSocketInputBase& inputSocket) noexcept(false) override
        {
            ConnectionBase::connect(inputSocket.castToType<Data, true>());
        }

        void connect(LogicSocketOutputBase& outputSocket) noexcept(false) override
        {
            ConnectionBase::connect(outputSocket.castToType<Data, true>());
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