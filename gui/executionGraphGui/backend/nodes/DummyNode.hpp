// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Fri Jul 06 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <rttr/registration>
#include <rttr/type>
#include <executionGraph/nodes/LogicCommon.hpp>

//! Stupid dummy Node for testing.
template<typename TConfig>
class DummyNode : public TConfig::NodeBaseType
{
public:
    EG_DEFINE_TYPES();
    using Base = typename Config::NodeBaseType;

private:
    RTTR_ENABLE()

public:
    //! Class which is used as a static, for RTTR Registration
    struct AutoRegisterRTTR
    {
        AutoRegisterRTTR(const std::string& rttiPostFix)
        {
            rttr::registration::class_<DummyNode>("DummyNode-" + rttiPostFix)
                .template constructor<NodeId>()(
                    rttr::policy::ctor::as_raw_ptr);
        }
    };

public:
    //@{
    // Input/Output Definitions
    enum Ins
    {
        Value1,
        Value2
    };
    enum Outs
    {
        Result1,
    };
    EG_DEFINE_SOCKET_TRAITS(Ins, Outs)
    using InSockets = InSocketDeclList<InSocketDecl<Value1, int>,
                                       InSocketDecl<Value2, int>>;

    using OutSockets = OutSocketDeclList<OutSocketDecl<Result1, int>>;
    EG_DEFINE_LOGIC_NODE_VALUE_GETTERS(Ins, InSockets, Outs, OutSockets)
    EG_DEFINE_STATIC_IN_SOCKET_NAMES(InSockets, "Val1", "Val2")
    EG_DEFINE_STATIC_OUT_SOCKET_NAMES(OutSockets, "Res")
    //@}

    template<typename... Args>
    DummyNode(Args&&... args)
        : Base(std::forward<Args>(args)...)
    {
        // Add all sockets
        this->template addSockets<InSockets>();
        this->template addSockets<OutSockets>(std::make_tuple(0));
    }

    void reset() override{};

    void compute() override
    {
        getOutVal<Result1>() = getInVal<Value1>() + getInVal<Value2>();
    }
};
