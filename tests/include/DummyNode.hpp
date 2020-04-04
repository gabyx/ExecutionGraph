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
    template<typename T>
    class DummyNode : public LogicNode
    {
        using Base = LogicNode;
        EG_DEFINE_NODE(DummyNode);

    public:
        EG_DEFINE_INPUT_DESC(in0Decl, T, 0, "Value0"_cs);
        EG_DEFINE_INPUT_DESC(in1Decl, float, 1, "Value1"_cs);
        EG_DEFINE_INPUT_DESC(in2Decl, double, 2, "Value2"_cs);

    private:
        EG_DEFINE_DESCS(inDecls, in0Decl, in2Decl, in1Decl);
        InputSocketsTuple<inDecls> m_inSockets;

    public:
        EG_DEFINE_OUTPUT_DESC(out0Decl, T, 0, "Value0"_cs);
        EG_DEFINE_OUTPUT_DESC(out1Decl, float, 1, "Value1"_cs);
        EG_DEFINE_OUTPUT_DESC(out2Decl, double, 2, "Value2"_cs);

    private:
        EG_DEFINE_DESCS(outDecls, out1Decl, out2Decl, out0Decl);
        OutputSocketsTuple<outDecls> m_outSockets;

    public:
        template<typename... Args>
        DummyNode(Args&&... args)
            : executionGraph::LogicNode(std::forward<Args>(args)...)
            , m_inSockets(makeSockets<inDecls>(*this))
            , m_outSockets(makeSockets<outDecls>(*this))
        {
            registerInputs(m_inSockets);
            registerOutputs(m_outSockets);
        }

        ~DummyNode() = default;

    public:
        EG_DEFINE_SOCKET_GETTERS(Node, m_inSockets, m_outSockets);
        //EG_DEFINE_SOCKET_CHECKS
    public:
        void init() override {}

        void reset() override {}

        void compute() override
        {
            auto handles = makeHandles<in0Decl,
                                       in1Decl,
                                       in2Decl,
                                       out0Decl,
                                       out1Decl,
                                       out2Decl>();
            invoke(handles,
                   [](auto& in0,
                      auto& in1,
                      auto& in2,
                      auto& out0,
                      auto& out1,
                      auto& out2) {
                       out0 = in0 + 1;
                       out1 = in1 + 1.f;
                       out2 = in2 + 1.0;
                   });
        }
    };
}  // namespace executionGraph
