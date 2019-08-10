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
    template<typename Tuple>
    constexpr auto makeSorted(Tuple&& t)
    {
        using A  = std::array<IndexType, std::tuple_size_v<Tuple>>;
        auto idx = tupleUtil::invoke(t, [](auto... a) { return A{a.m_index...}; });
        //std::sort(begin(idx), end(idx), [](auto& a, auto& b) { return a < b; });

        return idx;
    }

    template<typename TData,
             typename TNode,
             typename TIndex,
             typename IsInput>
    struct SocketDescription
    {
    public:
        using Data       = TData;
        using SocketType = LogicSocketInput<Data>;
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
        constexpr auto sortDescriptions(const std::tuple<SocketDesc...>& descs)
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

    template<typename... InputDesc,
             typename Node,
             std::enable_if_t<(... && meta::is<std::remove_cvref_t<InputDesc>,
                                               SocketDescription>::value),
                              int> = 0>
    auto makeSockets(const std::tuple<InputDesc...>& descs, Node& node)
    {
        using namespace makeSocketsDetails;
        // The SocketDescriptions::Index can be in any order
        // -> sort them and insert the sockets in order.

        static_assert(allSameNode<InputDesc...>(),
                      "You cannot mix descriptions for different nodes!");

        using Indices = decltype(sortDescriptions(descs));
        return tupleUtil::invoke<Indices>(descs,
                                          [&](auto&&... desc) {
                                              return std::make_tuple(
                                                  typename naked<decltype(desc)>::SocketType(desc.index(),
                                                                                             node)...);
                                          });
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

        static constexpr auto insDecl = std::forward_as_tuple(in2Decl,
                                                              in1Decl,
                                                              in0Decl);

    private:
        using Out = executionGraph::LogicSocketOutput<int>;
        std::tuple<Out, Out, Out> m_outSockets;

    public:
        static constexpr auto out0Decl = makeOutputDesc<0>("Value0", &DummyNode::m_outSockets);
        static constexpr auto out2Decl = makeOutputDesc<2>("Value2", &DummyNode::m_outSockets);
        static constexpr auto out1Decl = makeOutputDesc<1>("Value1", &DummyNode::m_outSockets);
        static constexpr auto outsDecl = std::forward_as_tuple(out0Decl,
                                                               out2Decl,
                                                               out1Decl);

    public:
        template<typename... Args>
        DummyNode(Args&&... args)
            : executionGraph::LogicNode(std::forward<Args>(args)...)
            , m_inSockets(makeSockets(insDecl, *this))
            , m_outSockets({Out{1123, 0, *this},
                            Out{11, 1, *this},
                            Out{55, 2, *this}})
        {
            // // register in node.
            // if(std::tuple_size_v<decltype(m_inSockets)> != 0)
            // {
            //     IndexType minIdx = std::numeric_limits<IndexType>::max();
            //     IndexType maxIdx = 0;
            //     tupleUtil::forEach(allDecl, [](auto&& decl) {
            //         minIdx = std::min(minIdx, decl.m_index);
            //         maxIdx = std::min(maxIdx, decl.m_index);
            //     });
            //     EXECGRAPH_THROW_IF(maxIdx - minIdx == std::tuple_size_v<decltype(allDecl)>, "Wrong!");

            //     tupleUtil::forEachIdx(m_inSockets,
            //                           [&](auto&& socket, auto socketIdx) {
            //                               //m_inputs[socketIdx] = &socket;
            //                           });
            // }
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
