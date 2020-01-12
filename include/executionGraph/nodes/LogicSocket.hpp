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
#include "executionGraph/common/ConstExprAlgos.hpp"
#include "executionGraph/common/DemangleTypes.hpp"
#include "executionGraph/common/EnumClassHelper.hpp"
#include "executionGraph/common/TupleUtil.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraph/nodes/LogicNode.hpp"
#include "executionGraph/nodes/LogicNodeDataBase.hpp"
#include "executionGraph/nodes/LogicSocketBase.hpp"
#include "executionGraph/nodes/LogicSocketDescription.hpp"

namespace executionGraph
{
    template<typename TTraits, typename Parent>
    class LogicSocketConnections
    {
    public:
        using NodeData            = typename TTraits::NodeData;
        using NodeDataConnections = typename TTraits::NodeDataConnections;

        friend NodeDataConnections;

    public:
        LogicSocketConnections(Parent& parent) noexcept
            : m_parent(parent){};

        ~LogicSocketConnections() noexcept
        {
            disconnect();
        }

    public:
        //! Connect a data node.
        void connect(NodeDataConnections& nodeData) noexcept
        {
            disconnect();
            onConnect(nodeData);
            m_nodeData->onConnect(parent());
        }

        //! Disconnect the data node.
        void disconnect() noexcept
        {
            if(m_nodeData)
            {
                m_nodeData->onDisconnect(parent());
                onDisconnect();
            }
        }

        NodeData* nodeData()
        {
            return isConnected() ? &m_nodeData->parent() : nullptr;
        }
        const NodeData* nodeData() const { return const_cast<LogicSocketConnections&>(*this).nodeData(); }

        bool isConnected() { return m_nodeData != nullptr; }

    protected:
        Parent& parent() { return m_parent; }
        const Parent& parent() const { return m_parent; }

        void onConnect(const NodeDataConnections& nodeData) noexcept
        {
            m_nodeData = const_cast<NodeDataConnections*>(&nodeData);
        }

        void onDisconnect() noexcept
        {
            m_nodeData = nullptr;
        }

    private:
        NodeDataConnections* m_nodeData = nullptr;  //! Connected data node.
        Parent& m_parent;
    };

    /* ---------------------------------------------------------------------------------------*/
    /*!
        The input socket.

        @date Sat Dec 28 2019
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TData>
    class LogicSocketInput final : public LogicSocketInputBase
    {
    public:
        EG_DEFINE_TYPES();
        using Base        = LogicSocketInputBase;
        using Data        = TData;
        using Connections = ConnectionTraits<Data>::InputSocketConnection;

    public:
        template<typename... Args>
        LogicSocketInput(Args&&... args)
            : LogicSocketInputBase(rttr::type::get<Data>(), std::forward<Args>(args)...)
            , m_connections(*this)
        {
        }

        ~LogicSocketInput() noexcept = default;

        //! Copy not allowed (since parent pointer)
        LogicSocketInput(const LogicSocketInput& other) = delete;
        LogicSocketInput& operator=(const LogicSocketInput& other) = delete;

        //! Move allowed
        LogicSocketInput(LogicSocketInput&& other) = default;
        LogicSocketInput& operator=(LogicSocketInput&& other) = default;

    public:
        auto dataHandle() const
        {
            EG_ASSERT_MSG(m_connections.isConnected(), "Socket not connected");
            return m_connections.nodeData()->dataHandleConst();
        }
        auto dataHandle()
        {
            EG_ASSERT_MSG(m_connections.isConnected(), "Socket not connected");
            return m_connections.nodeData()->dataHandleConst();
        }

    public:
        template<typename T>
        void connect(T& nodeData)
        {
            m_connections.connect(nodeData.connections());
        }

        void disconnect() noexcept override
        {
            m_connections.disconnect();
        }

        auto& connections() noexcept
        {
            return m_connections;
        }

        const auto& connections() const noexcept
        {
            return m_connections;
        }

    private:
        using Base::connect;

    private:
        Connections m_connections;
    };

    /* ---------------------------------------------------------------------------------------*/
    /*
        The output socket.

        @date Sat Dec 28 2019
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TData>
    class LogicSocketOutput final : public LogicSocketOutputBase
    {
    public:
        EG_DEFINE_TYPES();
        using Base        = LogicSocketOutputBase;
        using Data        = TData;
        using Connections = ConnectionTraits<Data>::OutputSocketConnection;

    public:
        template<typename... Args>
        LogicSocketOutput(Args&&... args)
            : LogicSocketOutputBase(rttr::type::get<Data>(), std::forward<Args>(args)...)
            , m_connections(*this)
        {
        }

        ~LogicSocketOutput() noexcept = default;

        //! Copy not allowed (since parent pointer)
        LogicSocketOutput(const LogicSocketOutput& other) = delete;
        LogicSocketOutput& operator=(const LogicSocketOutput& other) = delete;

        //! Move allowed
        LogicSocketOutput(LogicSocketOutput&& other) = default;
        LogicSocketOutput& operator=(LogicSocketOutput&& other) = default;

    public:
        auto dataHandle() const
        {
            EG_ASSERT(m_connections.isConnected(), "Socket not connected");
            return m_connections.nodeData()->dataHandleConst();
        }
        auto dataHandle()
        {
            EG_ASSERT(m_connections.isConnected(), "Socket not connected");
            return m_connections.nodeData()->dataHandleConst();
        }

    public:
        template<typename T>
        void connect(T& nodeData)
        {
            m_connections.connect(nodeData.connections());
        }

        void disconnect() noexcept override
        {
            m_connections.disconnect();
        }

        auto& connections() noexcept
        {
            return m_connections;
        }

        const auto& connections() const noexcept
        {
            return m_connections;
        }

    private:
        using Base::connect;

    private:
        Connections m_connections;
    };

    template<auto socketDesc>
    using SocketType = meta::if_<meta::bool_<socketDesc.isInput()>,
                                 LogicSocketInput<typename naked<decltype(socketDesc)>::Data>,
                                 LogicSocketOutput<typename naked<decltype(socketDesc)>::Data>>;

    namespace details
    {
        template<std::size_t I, auto& descs, typename... Args>
        auto makeSocket(Args&&... args)
        {
            constexpr auto& desc = std::get<I>(descs);
            using S              = meta::if_<meta::bool_<desc.isInput()>,
                                LogicSocketInput<typename naked<decltype(desc)>::Data>,
                                LogicSocketOutput<typename naked<decltype(desc)>::Data>>;
            return S{desc.index(), std::forward<Args>(args)...};
        }

    }  // namespace details

    //! Make a container for input socket from `LogicSocketDescription`s.
    //! @return A `std::tuple` of `LogicSocket<...>`.
    template<auto& descs,
             typename Node,
             typename Tuple = naked<decltype(descs)>,
             EG_ENABLE_IF((meta::is<Tuple, std::tuple>::value) &&
                          std::is_constant_evaluated())>
    auto makeSockets(const Node& node)
    {
        constexpr bool check = tupleUtil::invoke(
            descs,
            [](auto&&... ds) {
                std::array indices = {ds.index()...};
                return belongSocketDescriptionsToSameNodes(ds...) &&
                       (isInputDescriptions(ds...) || isOutputDescriptions(ds...)) &&
                       cx::is_sorted(indices.begin(), indices.end());
            });
        static_assert(check,
                      "Don't mix descriptions for different nodes "
                      "and only provide all input or all output descriptions "
                      "sorted by socket index.");

        return tupleUtil::indexed(
            descs,
            [&]<std::size_t... I>(auto&, std::index_sequence<I...>) {
                return std::make_tuple(details::makeSocket<I, descs>(node)...);
            });
    }

    //! The container type for input sockets used in specific node implementation.
    //! A `std::tuple` of `LogicSocket<...>`.
    template<auto& inputDescs>
    using InputSocketsTuple = decltype(makeSockets<inputDescs>(std::declval<LogicNode>()));

    //! The container type for output sockets used in specific node implementation.
    //! A `std::tuple` of `LogicSocket<...>`.
    template<auto& outputDescs>
    using OutputSocketsTuple = decltype(makeSockets<outputDescs>(std::declval<LogicNode>()));

    //! Convert a tuple of sockets into a container `Container`
    //! of pointers pointing to the passed sockets.
    //! Used to registers sockets
    template<typename Container, typename... Type>
    auto makePtrList(std::tuple<Type...>& sockets)
    {
        using Pointer = typename Container::value_type;
        static_assert(std::is_pointer_v<Pointer>,
                      "Value type needs to be pointer");
        return tupleUtil::invoke(
            sockets,
            [](auto&... socket) {
                return Container{Pointer{&socket}...};
            });
    }

    //! Get an input socket of a tuple-like socket-container
    //! from a socket description `LogicSocketDescription`
    template<auto& socketDesc,
             typename Sockets,
             typename SocketDesc = naked<decltype(socketDesc)>,
             EG_ENABLE_IF((meta::is_v<Sockets, std::tuple> &&
                           isInputDescriptions(socketDesc)))>
    auto& getInputSocket(Sockets& sockets)
    {
        return std::get<socketDesc.index()>(sockets);
    }

    //! Get an output socket of a tuple-like socket-container
    //! from a socket description `LogicSocketDescription`
    template<auto& socketDesc,
             typename Sockets,
             typename SocketDesc = naked<decltype(socketDesc)>,
             EG_ENABLE_IF((meta::is_v<Sockets, std::tuple> &&
                           isOutputDescriptions(socketDesc)))>
    auto& getOutputSocket(Sockets& sockets)
    {
        return std::get<socketDesc.index()>(sockets);
    }

#define EG_DEFINE_SOCKET_GETTERS(Node, inputSockets, outputSockets)        \
    template<auto& socketDesc,                                             \
             typename SocketDesc = naked<decltype(socketDesc)>,            \
             EG_ENABLE_IF((isInputDescriptions(socketDesc) &&              \
                           SocketDesc::template belongsToNode<Node>()))>   \
    auto& socket()                                                         \
    {                                                                      \
        return executionGraph::getInputSocket<socketDesc>(inputSockets);   \
    }                                                                      \
    template<auto& socketDesc,                                             \
             typename SocketDesc = naked<decltype(socketDesc)>,            \
             EG_ENABLE_IF((isOutputDescriptions(socketDesc) &&             \
                           SocketDesc::template belongsToNode<Node>()))>   \
    auto& socket()                                                         \
    {                                                                      \
        return executionGraph::getOutputSocket<socketDesc>(outputSockets); \
    }                                                                      \
    using __FILE__##__LINE__##FORCE_SEMICOLON = int

}  // namespace executionGraph