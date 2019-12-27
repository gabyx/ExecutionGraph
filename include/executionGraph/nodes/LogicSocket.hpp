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
#include "executionGraph/common/TupleUtil.hpp"
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

    protected:
        template<typename... Args>
        LogicSocketInputBase(Args&&... args) noexcept
            : LogicSocketBase(std::forward<Args>(args)...)
        {
        }

        ~LogicSocketInputBase() noexcept = default;

    public:
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

        const LogicNodeDataBase* dataNode() { return m_nodeData; }

    protected:
        const LogicNodeDataBase* m_nodeData = nullptr;  //! Connected data node.
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

        ~LogicSocketOutputBase() noexcept = default;

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
        using Data     = TData;
        using NodeData = LogicNodeData<Data>;

    public:
        template<typename... Args>
        LogicSocketInput(Args&&... args)
            : LogicSocketInputBase(rttr::type::get<Data>(), std::forward<Args>(args)...)
        {
        }

        ~LogicSocketInput() noexcept = default;

        //! Copy not allowed (since parent pointer)
        LogicSocketInput(const LogicSocketInput& other) = delete;
        LogicSocketInput& operator=(const LogicSocketInput& other) = delete;

        //! Move allowed
        LogicSocketInput(LogicSocketInput&& other) = default;
        LogicSocketInput& operator=(LogicSocketInput&& other) = default;

        //! Connect a data node.
        bool connect(const NodeData& nodeData)
        {
            m_nodeData = &nodeData;
            const_expr<NodeData*>(m_nodeData)->onAddGetLink(*this);
        }

        //! Disconnect the data node.
        bool disconnect()
        {
            const_expr<NodeData*>(m_nodeData)->onRemoveGetLink(*this);
            m_nodeData = nullptr;
        }

        auto data() {}

        const NodeData* dataNode() { return m_nodeData; }

    protected:
        const NodeData* m_nodeData = nullptr;  //! Connected data node.
    };

    template<typename TData>
    class LogicSocketOutput final : public LogicSocketOutputBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();
        using Data     = TData;
        using NodeData = LogicNodeData<Data>;

    public:
        template<typename... Args>
        LogicSocketOutput(Args&&... args)
            : LogicSocketOutputBase(rttr::type::get<Data>(), std::forward<Args>(args)...)
        {
        }

        ~LogicSocketOutput() noexcept = default;

        //! Copy not allowed (since parent pointer)
        LogicSocketOutput(const LogicSocketOutput& other) = delete;
        LogicSocketOutput& operator=(const LogicSocketOutput& other) = delete;

        //! Move allowed
        LogicSocketOutput(LogicSocketOutput&& other) = default;
        LogicSocketOutput& operator=(LogicSocketOutput&& other) = default;

        //! Connect a data node.
        bool connect(NodeData& nodeData)
        {
            m_nodeData = &nodeData;
            m_nodeData->onAddWriteLink(*this);
        }

        //! Disconnect the data node.
        bool disconnect()
        {
            m_nodeData->onRemoveWriteLink(*this);
            m_nodeData = nullptr;
        }

        NodeData* dataNode() { return m_nodeData; }

    protected:
        NodeData* m_nodeData = nullptr;  //! Connected data node.
    };

    namespace makeSocketsDetails
    {
        template<typename A, typename B>
        using comp = meta::less<typename meta::at_c<A, 1>::Index,
                                typename meta::at_c<B, 1>::Index>;

        template<typename A>
        using getIdx = typename meta::at_c<A, 1>::Index;

        template<typename T>
        using naked = std::remove_cvref_t<T>;

        //! Test if all descriptions are input or output descriptions
        template<typename... SocketDesc>
        constexpr bool eitherInputsOrOutputs()
        {
            using namespace makeSocketsDetails;
            using namespace meta;
            return (... && naked<SocketDesc>::isInput()) ||
                   (... && naked<SocketDesc>::isOutput());
        }

        //! Test if all description have the same node type.
        template<typename... SocketDesc>
        constexpr bool allSameNode()
        {
            using namespace makeSocketsDetails;
            using namespace meta;
            using List = list<naked<SocketDesc>...>;
            return (... && std::is_same_v<typename naked<SocketDesc>::Node,
                                          typename at_c<List, 0>::Node>);
        }

        //! Returns the indices list denoting the sorted descriptions `descs`.
        template<bool checkIncreasingByOne = true,
                 typename... SocketDesc>
        constexpr auto sortDescriptions()
        {
            constexpr auto N = sizeof...(SocketDesc);

            using namespace makeSocketsDetails;
            using namespace meta::placeholders;
            using namespace meta;

            using List = list<naked<SocketDesc>...>;

            using EnumeratedList = zip<list<as_list<make_index_sequence<N>>, List>>;

            // Sort the list
            using Sorted = sort<EnumeratedList,
                                lambda<_a, _b, defer<comp, _a, _b>>>;

            static_assert(!checkIncreasingByOne ||
                              std::is_same_v<transform<Sorted, quote<getIdx>>,
                                             as_list<make_index_sequence<N>>>,
                          "Socket description indices are not monotone increasing by one");

            using SortedIndices = unique<transform<Sorted,
                                                   bind_back<quote<at>, meta::size_t<0>>>>;

            return to_index_sequence<SortedIndices>{};
        }
    };  // namespace makeSocketsDetails

    //! Make input sockets from `InputSocketDescriptions`.
    template<typename... Descriptions,
             typename Node>
    auto makeSockets(const std::tuple<Descriptions&...>& descs,
                     const Node& node)
    {
        static_assert(makeSocketsDetails::eitherInputsOrOutputs<Descriptions...>(),
                      "Either all inputs or outputs!");

        using namespace makeSocketsDetails;
        // The SocketDescriptions::Index can be in any order
        // -> sort them and insert the sockets in order.

        static_assert(allSameNode<Descriptions...>(),
                      "You cannot mix descriptions for different nodes!");

        constexpr auto indices = sortDescriptions<true, Descriptions...>();
        return tupleUtil::invoke(
            descs,
            [&](auto&&... desc) {
                return std::make_tuple(
                    typename naked<decltype(desc)>::SocketType(desc.index(), node)...);
            },
            indices);
    }

    template<typename InputDescs>
    using InputSocketsType = decltype(makeSockets(std::declval<InputDescs>(),
                                                  std::declval<LogicNode>()));
    template<typename OutputDescs>
    using OutputSocketsType = decltype(makeSockets(std::declval<OutputDescs>(),
                                                   std::declval<LogicNode>()));

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

}  // namespace executionGraph