// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Jun 06 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <meta/meta.hpp>
#include <executionGraph/common/Exception.hpp>
#include <executionGraph/common/MetaIndices.hpp>
#include <executionGraph/common/MetaInvoke.hpp>
#include <executionGraph/common/TypeDefs.hpp>
#include <executionGraph/config/Config.hpp>
#include <executionGraph/nodes/LogicNode.hpp>
#include <executionGraph/nodes/LogicSocket.hpp>

namespace executionGraph
{
    template<typename TData,
             typename TNode,
             typename TIndex,
             typename IsInput>
    struct SocketDescription
    {
    public:
        using Data       = TData;
        using SocketType = meta::if_<IsInput,
                                     LogicSocketInput<Data>,
                                     LogicSocketOutput<Data>>;
        using Node       = TNode;
        using Index      = TIndex;

        constexpr SocketDescription(std::string_view name)
            : m_name(name)
        {
        }

        constexpr const auto index() const
        {
            return static_cast<IndexType>(Index::value);
        }
        constexpr auto& name() const { return m_name; }

        static constexpr auto isInput() { return IsInput::value; }
        static constexpr auto isOutput() { return !isInput(); }

    private:
        const std::string_view m_name;
    };

    template<typename TData,
             typename TNode,
             typename TIndex>
    using InputDescription = SocketDescription<TData,
                                               TNode,
                                               TIndex,
                                               meta::bool_<true>>;
    template<typename TData,
             typename TNode,
             typename TIndex>
    using OutputDescription = SocketDescription<TData,
                                                TNode,
                                                TIndex,
                                                meta::bool_<false>>;

    template<typename CallableDesc,
             IndexType Idx,
             typename TNode,
             typename TSockets>
    constexpr auto makeSocketDesc(std::string_view name,
                                  TSockets TNode::*sockets)
    {
        static_assert(Idx < std::tuple_size_v<TSockets>, "Wrong index!");
        using Data = typename std::tuple_element_t<Idx, TSockets>::Data;
        return meta::invoke<CallableDesc,
                            Data,
                            TNode,
                            meta::size_t<Idx>>{name};
    }

    template<IndexType Idx,
             typename TNode,
             typename TSockets>
    constexpr auto makeInputDesc(std::string_view name,
                                 TSockets TNode::*sockets)
    {
        return makeSocketDesc<meta::quote<InputDescription>, Idx>(name, sockets);
    }

    template<IndexType Idx,
             typename TNode,
             typename TSockets>
    constexpr auto makeOutputDesc(std::string_view name,
                                  TSockets TNode::*sockets)
    {
        return makeSocketDesc<meta::quote<OutputDescription>, Idx>(name, sockets);
    }

    namespace makeSocketsDetails
    {
        template<typename A, typename B>
        using comp = meta::less<typename meta::at_c<A, 1>::Index,
                                typename meta::at_c<B, 1>::Index>;

        template<typename T>
        using getIdx = typename T::Index;

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
        template<bool requireSocketIndexSequence = true,
                 typename... SocketDesc>
        constexpr auto sortDescriptions()
        {
            constexpr auto N = sizeof...(SocketDesc);

            using namespace makeSocketsDetails;
            using namespace meta::placeholders;
            using namespace meta;

            using List = list<naked<SocketDesc>...>;

            static_assert(!requireSocketIndexSequence ||
                              unique<transform<List, quote<getIdx>>>::size() == N,
                          "Socket description indices are not monotone increasing by one");

            using EnumeratedList = zip<list<as_list<make_index_sequence<N>>, List>>;

            // Sort the list
            using Sorted = sort<EnumeratedList,
                                lambda<_a, _b, defer<comp, _a, _b>>>;

            using SortedIndices = unique<transform<Sorted,
                                                   bind_back<quote<at>, size_t<0>>>>;

            static_assert(!requireSocketIndexSequence ||
                              (at<List, front<SortedIndices>>::Index::value == 0 &&
                               at<List, back<SortedIndices>>::Index::value + 1 == N),
                          "Socket description indices are not monotone increasing by one");

            return to_index_sequence<SortedIndices>{};
        }
    };  // namespace makeSocketsDetails

    //! Make input sockets from `InputSocketDescriptions`.
    template<typename... InputDesc,
             typename Node,
             std::enable_if_t<(... && std::remove_cvref_t<InputDesc>::isInput()), int> = 0>
    auto makeSockets(std::tuple<InputDesc&...> descs,
                     Node& node)
    {
        using namespace makeSocketsDetails;
        // The SocketDescriptions::Index can be in any order
        // -> sort them and insert the sockets in order.

        static_assert(allSameNode<InputDesc...>(),
                      "You cannot mix descriptions for different nodes!");

        constexpr auto indices = sortDescriptions<true, InputDesc...>();
        return tupleUtil::invoke(descs,
                                 [&](auto&&... desc) {
                                     return std::make_tuple(
                                         typename naked<decltype(desc)>::SocketType(desc.index(), node)...);
                                 },
                                 indices);
    }

    template<typename... OutputDesc,
             typename... Data,
             typename Node,
             std::enable_if_t<(... && std::remove_cvref_t<OutputDesc>::isOutput()), int> = 0>
    auto makeSockets(std::tuple<OutputDesc&...> descs,
                     std::tuple<Data...> defaultValues,
                     Node& node)
    {
        static_assert((... && std::is_reference_v<Data>),
                      "Default values need to be lvalue/rvalue-References");

        using namespace makeSocketsDetails;
        // The SocketDescriptions::Index can be in any order
        // -> sort them and insert the sockets in order.

        static_assert(allSameNode<OutputDesc...>(),
                      "You cannot mix descriptions for different nodes!");

        constexpr auto indices = sortDescriptions<true, OutputDesc...>();

        auto params = tupleUtil::zipForward(descs, defaultValues);

        return tupleUtil::invoke(std::move(params), /* move: to properly forward with std::get below */
                                 [&](auto&&... descAndValue) {
                                     return std::make_tuple(
                                         typename naked<decltype(std::get<0>(descAndValue))>::SocketType(
                                             std::get<1>(descAndValue), std::get<0>(descAndValue).index(), node)...);
                                 },
                                 indices);
    }

    //! Stupid dummy Node for testing.
    class DummyNode : public LogicNode
    {
    private:
        using In = LogicSocketInput<int>;
        std::tuple<In, In, In> m_inSockets;

    public:
        static constexpr auto in0Decl = makeInputDesc<0>("Value0", &DummyNode::m_inSockets);
        static constexpr auto in2Decl = makeInputDesc<2>("Value2", &DummyNode::m_inSockets);
        static constexpr auto in1Decl = makeInputDesc<1>("Value1", &DummyNode::m_inSockets);

        static constexpr auto inDecls = std::forward_as_tuple(in2Decl,
                                                              in1Decl,
                                                              in0Decl);

    private:
        using Out = executionGraph::LogicSocketOutput<int>;
        std::tuple<Out, Out, Out> m_outSockets;

    public:
        static constexpr auto out0Decl = makeOutputDesc<0>("Value0", &DummyNode::m_outSockets);
        static constexpr auto out2Decl = makeOutputDesc<2>("Value2", &DummyNode::m_outSockets);
        static constexpr auto out1Decl = makeOutputDesc<1>("Value1", &DummyNode::m_outSockets);
        static constexpr auto outDecls = std::forward_as_tuple(out0Decl,
                                                               out2Decl,
                                                               out1Decl);

    public:
        template<typename... Args>
        DummyNode(Args&&... args)
            : executionGraph::LogicNode(std::forward<Args>(args)...)
            , m_inSockets(makeSockets(inDecls, *this))
            , m_outSockets(makeSockets(outDecls, std::forward_as_tuple(1123, 11, 44), *this))
        {
            std::get<in0Decl.index()>(m_inSockets);
        }

        // static constexpr SocketDescription<int> inDeclValue2{1, "Value2"};
        // static constexpr OutSocketDecleration<int> outDeclResult{0, "Result"};

        // using InSocketList  = decltype(getInSocketList(inDeclValue1, inDeclValue2));
        // using OutSocketList = decltype(getOutSocketList(outDeclResult));

        // template<typename SocketDecl>
        // auto& outSocket()
        // {
        //     using T = SocketDecl::Data;
        //     return std::get<getIndex>(m_outSockets);
        // }

        // template<typename SocketDecl>
        // auto& inSocket(const SocketDecl& decl)
        // {
        //     using T = SocketDecl::Data;
        //     return std::get<LogicSocketInput<T>>(m_inSockets);
        // }

        void reset() override{};

        void compute() override
        {
        }

    private:
        // InSocketList m_inSockets;
        // OutSocketList m_outSockets;
    };
}  // namespace executionGraph
