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
#include "executionGraph/common/Exception.hpp"
#include "executionGraph/common/MetaIndices.hpp"
#include "executionGraph/common/MetaInvoke.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/config/Config.hpp"
#include "executionGraph/nodes/LogicNode.hpp"
#include "executionGraph/nodes/LogicSocket.hpp"
#include "executionGraph/nodes/LogicSocketDescription.hpp"

namespace executionGraph
{
    //! Stupid dummy Node for testing.
    class DummyNode : public LogicNode
    {
        using Base = LogicNode;

    public:
        EXECGRAPH_DEFINE_INPUT_DESCR(in0Decl, int, 0, "Value0");
        EXECGRAPH_DEFINE_INPUT_DESCR(in2Decl, float, 2, "Value2");
        EXECGRAPH_DEFINE_INPUT_DESCR(in1Decl, double, 1, "Value1");

    private:
        EXECGRAPH_DEFINE_DESCS(inDecls, in0Decl, in2Decl, in1Decl);
        InputSocketsType<decltype(inDecls)> m_inSockets;

    public:
        EXECGRAPH_DEFINE_OUTPUT_DESCR(out0Decl, int, 0, "Value0");
        EXECGRAPH_DEFINE_OUTPUT_DESCR(out2Decl, float, 2, "Value2");
        EXECGRAPH_DEFINE_OUTPUT_DESCR(out1Decl, double, 1, "Value1");

    private:
        EXECGRAPH_DEFINE_DESCS(outDecls, out1Decl, out2Decl, out0Decl);
        OutputSocketsType<decltype(outDecls)> m_outSockets;

    public:
        template<typename... Args>
        DummyNode(Args&&... args)
            : executionGraph::LogicNode(std::forward<Args>(args)...)
            , m_inSockets(makeSockets(inDecls, *this))
            , m_outSockets(makeSockets(outDecls, *this))
        {
            registerInputs(m_inSockets);
            registerOutputs(m_outSockets);
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
