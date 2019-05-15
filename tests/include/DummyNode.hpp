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

#ifndef tests_DummmyNode_hpp
#define tests_DummmyNode_hpp

#include <executionGraph/common/Exception.hpp>
#include <executionGraph/common/TypeDefs.hpp>
#include <executionGraph/config/Config.hpp>

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

    template<typename T, typename TNode>
    struct InSocketDeclaration
    {
        constexpr InSocketDeclaration(IndexType index,
                                      std::string_view name)
            : m_index(index)
            , m_name(name)
        {}

        IndexType m_index;
        std::string_view m_name;
    };

    template<IndexType Idx, typename TNode, typename S>
    constexpr auto makeInputDesc(std::string_view name,
                                 S TNode::*sockets)
    {
        static_assert(Idx < std::tuple_size_v<S>, "Wrong index!");
        using T = std::tuple_element_t<Idx, S>;
        return InSocketDeclaration<T, TNode>{Idx, name};
    }

    //! Stupid dummy Node for testing.
    class DummyNode : public executionGraph::LogicNode
    {
    private:
        using In = executionGraph::LogicSocketInput<int>;

        std::tuple<In, In, In> m_inSockets;

        static constexpr auto in0Decl = makeInputDesc<0>("Value0", &DummyNode::m_inSockets);
        static constexpr auto in2Decl = makeInputDesc<2>("Value2", &DummyNode::m_inSockets);
        static constexpr auto in1Decl = makeInputDesc<1>("Value1", &DummyNode::m_inSockets);
        static constexpr auto allDecl = std::forward_as_tuple(in0Decl, in2Decl, in1Decl);

    public:
        template<typename... Args>
        DummyNode(Args&&... args)
            : executionGraph::LogicNode(std::forward<Args>(args)...)
            , m_inSockets({In{0, *this},
                           In{1, *this},
                           In{2, *this}})  // sequenziell
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

            std::get<in0Decl.m_index>(m_inSockets);
        }

        // static constexpr InSocketDeclaration<int> inDeclValue2{1, "Value2"};
        // static constexpr OutSocketDecleration<int> outDeclResult{0, "Result"};

        // using InSocketList  = decltype(getInSocketList(inDeclValue1, inDeclValue2));
        // using OutSocketList = decltype(getOutSocketList(outDeclResult));

        // template<typename SocketDecl>
        // auto& outSocket()
        // {
        //     using T = SocketDecl::DataType;
        //     return std::get<getIndex>(m_outSockets);
        // }

        // template<typename SocketDecl>
        // auto& inSocket(const SocketDecl& decl)
        // {
        //     using T = SocketDecl::DataType;
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
#endif