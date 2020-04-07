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
#include "executionGraph/common/StaticAssert.hpp"
#include "executionGraph/common/TupleUtil.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraph/nodes/LogicNode.hpp"
#include "executionGraph/nodes/LogicSocketBase.hpp"
#include "executionGraph/nodes/LogicSocketDataBase.hpp"
#include "executionGraph/nodes/LogicSocketDescription.hpp"

namespace executionGraph
{
    //! Wrapping functionality around connection on a socket.
    template<typename TTraits, typename Parent>
    class LogicSocketConnections final
    {
    public:
        using ISocketData            = typename TTraits::ISocketData;
        using ISocketDataConnections = typename TTraits::ISocketDataConnections;

        friend ISocketDataConnections;

    public:
        explicit LogicSocketConnections(Parent& parent) noexcept
            : m_parent(&parent){};

        ~LogicSocketConnections() noexcept
        {
            disconnect();
        }

        LogicSocketConnections(const LogicSocketConnections&) = delete;
        LogicSocketConnections& operator=(const LogicSocketConnections&) = delete;

        LogicSocketConnections(LogicSocketConnections&& other)
        {
            // Connections are not moved!
        }

        LogicSocketConnections& operator=(LogicSocketConnections&& other) = delete;

    public:
        void init(Parent& parent)
        {
            m_parent = &parent;
        }

        //! Connect a data node.
        void connect(ISocketDataConnections& dataConnections) noexcept
        {
            disconnect();
            onConnect(dataConnections);
            m_dataConnection->onConnect(parent());
        }

        //! Disconnect the data node.
        void disconnect() noexcept
        {
            if(m_dataConnection)
            {
                m_dataConnection->onDisconnect(parent());
                onDisconnect();
            }
        }

        bool isConnected() const { return m_dataConnection != nullptr; }

        ISocketData* socketData()
        {
            return isConnected() ? &m_dataConnection->parent() : nullptr;
        }
        const ISocketData* socketData() const { return const_cast<LogicSocketConnections&>(*this).socketData(); }

    public:
        const Parent& parent()
        {
            EG_ASSERT(m_parent, "Parent not set");
            return *m_parent;
        }
        const Parent& parent() const
        {
            return const_cast<LogicSocketConnections&>(*this).parent();
        }

    private:
        void onConnect(const ISocketDataConnections& socketData) noexcept
        {
            m_dataConnection = const_cast<ISocketDataConnections*>(&socketData);
        }

        void onDisconnect() noexcept
        {
            m_dataConnection = nullptr;
        }

    private:
        ISocketDataConnections* m_dataConnection = nullptr;  //! Connected data node.
        Parent* m_parent                         = nullptr;  //! Parent of this connection wrapper.
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
        using Connections = ConnectionTraits<Data>::InputSocketConnections;

    public:
        template<typename... Args>
        explicit LogicSocketInput(Args&&... args)
            : LogicSocketInputBase(rttr::type::get<Data>(), std::forward<Args>(args)...)
            , m_connections(*this)
        {
        }

        ~LogicSocketInput() noexcept = default;

        //! Copy not allowed (since parent pointer)
        LogicSocketInput(const LogicSocketInput& other) = delete;
        LogicSocketInput& operator=(const LogicSocketInput& other) = delete;

        //! Move allowed
        LogicSocketInput(LogicSocketInput&& other)
            : Base(std::move(other))
            , m_connections(*this)
        {
            // Connections are not moved!
        }
        LogicSocketInput& operator=(LogicSocketInput&& other) = delete;

    public:
        auto dataHandle() const
        {
            EG_ASSERT(m_connections.isConnected(), "Socket not connected");
            return m_connections.socketData()->template dataAccess<Data>().dataHandleConst();
        }
        auto dataHandle()
        {
            EG_ASSERT_MSG(m_connections.isConnected(), "Socket not connected");
            return m_connections.socketData()->template dataAccess<Data>().dataHandleConst();
        }

    public:
        template<typename T>
        void connect(T& socketData)
        {
            m_connections.connect(socketData.connections());
        }

        void disconnect() noexcept override
        {
            m_connections.disconnect();
        }

        Connections& connections() noexcept
        {
            return m_connections;
        }

        const Connections& connections() const noexcept
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
        using Connections = ConnectionTraits<Data>::OutputSocketConnections;

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
        LogicSocketOutput(LogicSocketOutput&& other)
            : Base(std::move(other))
            , m_connections(*this)
        {
            // Connections are not moved!
        }
        LogicSocketOutput& operator=(LogicSocketOutput&& other) = delete;

    public:
        auto dataHandle() const
        {
            EG_ASSERT(m_connections.isConnected(), "Socket not connected");
            return m_connections.socketData()->template dataAccess<Data>().dataHandleConst();
        }
        auto dataHandle()
        {
            EG_ASSERT(m_connections.isConnected(), "Socket not connected");
            return m_connections.socketData()->template dataAccess<Data>().dataHandle();
        }

    public:
        template<typename T>
        void connect(T& socketData)
        {
            m_connections.connect(socketData.connections());
        }

        void disconnect() noexcept override
        {
            m_connections.disconnect();
        }

        Connections& connections() noexcept
        {
            return m_connections;
        }

        const Connections& connections() const noexcept
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
            using S              = SocketType<desc>;
            return S{desc.index(), std::forward<Args>(args)...};
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

        template<auto& desc,
                 typename InSockets,
                 typename OutSockets>
        decltype(auto) getSocket(const InSockets& inSockets,
                                 OutSockets& outSockets)
        {
            if constexpr(desc.isInput())
            {
                return std::get<desc.index()>(inSockets);
            }
            else
            {
                return std::get<desc.index()>(outSockets);
            }
        }

        template<auto& descs,
                 typename InSockets,
                 typename OutSockets>
        auto makeHandles(const InSockets& inSockets, OutSockets& outSockets)
        {
            return tupleUtil::indexed(
                descs,
                [&]<std::size_t... I>(auto&, std::index_sequence<I...>) {
                    return std::make_tuple(
                        details::getSocket<std::get<I>(descs)>(inSockets, outSockets).dataHandle()...);
                });
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
        EG_STATIC_ASSERT(check,
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

#define EG_DEFINE_SOCKET_GETTERS(Node, inputSockets, outputSockets)                     \
    template<auto& socketDesc,                                                          \
             typename SocketDesc = naked<decltype(socketDesc)>>                         \
    auto& socket()                                                                      \
    {                                                                                   \
        EG_STATIC_ASSERT(SocketDesc::template belongsToNode<Node>(),                    \
                         "Description does not belong to this node");                   \
                                                                                        \
        if constexpr(isInputDescriptions(socketDesc))                                   \
        {                                                                               \
            return executionGraph::details::getInputSocket<socketDesc>(inputSockets);   \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            return executionGraph::details::getOutputSocket<socketDesc>(outputSockets); \
        }                                                                               \
    }                                                                                   \
                                                                                        \
    template<auto& socketDesc,                                                          \
             typename SocketDesc = naked<decltype(socketDesc)>>                         \
    auto& handle()                                                                      \
    {                                                                                   \
        EG_STATIC_ASSERT(SocketDesc::template belongsToNode<Node>(),                    \
                         "Descriptions does not belong to this node");                  \
                                                                                        \
        return socket<socketDesc>().dataHandle();                                       \
    }                                                                                   \
                                                                                        \
    template<auto&... descs>                                                            \
    auto makeHandles()                                                                  \
    {                                                                                   \
        static constexpr auto t = std::forward_as_tuple(descs...);                      \
        return details::makeHandles<t>(                                                 \
            inputSockets,                                                               \
            outputSockets);                                                             \
    }                                                                                   \
    ASSERT_SEMICOLON_DECL

}  // namespace executionGraph