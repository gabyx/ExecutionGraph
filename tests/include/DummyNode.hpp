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
             typename TIndex,
             typename IsInput,
             typename TNode = void>
    struct SocketDescription
    {
    public:
        using Data       = TData;
        using SocketType = meta::if_<IsInput,
                                     LogicSocketInput<Data>,
                                     LogicSocketOutput<Data>>;
        using Index      = TIndex;
        using Node       = TNode;

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
        //! @todo This needs c++20 constexpr std::string ...
        const std::string_view m_name;
    };

    template<typename TData,
             typename TIndex,
             typename TNode = void>
    using InputDescription = SocketDescription<TData,
                                               TIndex,
                                               meta::bool_<true>,
                                               TNode>;
    template<typename TData,
             typename TIndex,
             typename TNode = void>
    using OutputDescription = SocketDescription<TData,
                                                TIndex,
                                                meta::bool_<false>,
                                                TNode>;

    template<typename CallableDesc,
             typename Data,
             IndexType Idx,
             typename TNode = void>
    constexpr auto makeSocketDesc(std::string_view name)
    {
        return meta::invoke<CallableDesc,
                            Data,
                            meta::size_t<Idx>,
                            TNode>{std::move(name)};
    }

    template<typename TData,
             IndexType Idx>
    constexpr auto makeInputDesc(std::string_view name)
    {
        return makeSocketDesc<meta::quote<InputDescription>, TData, Idx>(std::move(name));
    }

    template<typename TData,
             IndexType Idx>
    constexpr auto makeOutputDesc(std::string_view name)
    {
        return makeSocketDesc<meta::quote<OutputDescription>, TData, Idx>(std::move(name));
    }

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
    template<typename... Description,
             typename Node,
             std::enable_if_t<makeSocketsDetails::eitherInputsOrOutputs<Description...>(), int> = 0>
    auto makeSockets(const std::tuple<Description&...>& descs,
                     const Node& node)
    {
        using namespace makeSocketsDetails;
        // The SocketDescriptions::Index can be in any order
        // -> sort them and insert the sockets in order.

        static_assert(allSameNode<Description...>(),
                      "You cannot mix descriptions for different nodes!");

        constexpr auto indices = sortDescriptions<true, Description...>();
        return tupleUtil::invoke(descs,
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

    //! Stupid dummy Node for testing.
    class DummyNode : public LogicNode
    {
    public:
        static constexpr auto in0Decl = makeInputDesc<int, 0>("Value0");
        static constexpr auto in2Decl = makeInputDesc<float, 2>("Value2");
        static constexpr auto in1Decl = makeInputDesc<double, 1>("Value1");

    private:
        static constexpr auto inDecls = std::forward_as_tuple(in0Decl, in2Decl, in1Decl);
        InputSocketsType<decltype(inDecls)> m_inSockets;

    public:
        static constexpr auto out0Decl = makeOutputDesc<int, 0>("Value0");
        static constexpr auto out2Decl = makeOutputDesc<float, 2>("Value2");
        static constexpr auto out1Decl = makeOutputDesc<double, 1>("Value1");

    private:
        static constexpr auto outDecls = std::forward_as_tuple(out0Decl, out2Decl, out1Decl);
        OutputSocketsType<decltype(outDecls)> m_outSockets;

    public:
        template<typename... Args>
        DummyNode(Args&&... args)
            : executionGraph::LogicNode(std::forward<Args>(args)...)
            , m_inSockets(makeSockets(inDecls, *this))
            , m_outSockets(makeSockets(outDecls, *this))
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
