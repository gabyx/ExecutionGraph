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

#include <rttr/registration>
#include <executionGraph/common/TypeDefs.hpp>
#include <executionGraph/config/Config.hpp>

//! Stupid dummy Node for testing.
template<typename TConfig>
class DummyNode : public TConfig::NodeBaseType
{
public:
    EXECGRAPH_DEFINE_CONFIG(TConfig);
    using Base = typename Config::NodeBaseType;

private:
    RTTR_ENABLE(Base)
public:
    struct AutoRegisterRTTR
    {
        AutoRegisterRTTR()
        {
            rttr::registration::class_<DummyNode>("DummyNode")
                .template constructor<NodeId, const std::string&>()(
                    rttr::policy::ctor::as_raw_ptr)
                .template constructor<NodeId>()(
                    rttr::policy::ctor::as_raw_ptr);
        }
    };

public:
    enum Ins
    {
        Value1,
        Value2,
        Value3NotAdded
    };
    enum Outs
    {
        Result1,
    };
    EXECGRAPH_DEFINE_SOCKET_TRAITS(Ins, Outs)

    using InSockets = InSocketDeclList<InSocketDecl<Value1, int>,
                                       InSocketDecl<Value2, int>>;

    using OutSockets = OutSocketDeclList<OutSocketDecl<Result1, int>>;

    EXECGRAPH_DEFINE_LOGIC_NODE_GET_TYPENAME()
    EXECGRAPH_DEFINE_LOGIC_NODE_VALUE_GETTERS(Ins, InSockets, Outs, OutSockets)

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
        // ugly syntax due to template shit
        //        this->template getValue<typename OutSockets::template Get<Result1>>() =
        //            this->template getValue<typename InSockets::template Get<Value1>>() +
        //            this->template getValue<typename InSockets::template Get<Value2>>();
        getOutVal<Result1>() = getInVal<Value1>() + getInVal<Value2>(); 
        /* + getInVal<Value3NotAdded>() // would be a compile error */
    }
};

#endif