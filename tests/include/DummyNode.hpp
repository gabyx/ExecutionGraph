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

#include <executionGraph/common/TypeDefs.hpp>
#include <executionGraph/config/Config.hpp>

struct InSocketDecleration
{
    constexpr InSocketDecleration(IndexType index,
                                  std::string_view name)
        : m_index(index), m_name(name) {}

    IndexType m_index;
    std::string_view m_name;
}

//! Stupid dummy Node for testing.
class DummyNode : public LogicNode
{
public:
    static constexpr InSocketDecleration<int> inDeclValue1{0, "Value1"};
    static constexpr InSocketDecleration<int> inDeclValue2{1, "Value2"};
    static constexpr OutSocketDecleration<int> outDeclResult{0, "Result"};

    using InSocketList  = decltype(getInSocketList(inDeclValue1, inDeclValue2));
    using OutSocketList = decltype(getOutSocketList(outDeclResult));

    template<typename SocketDecl>
    auto& outSocket()
    {
        using T = SocketDecl::DataType;
        return std::get<getIndex>(m_outSockets);
    }

    template<typename SocketDecl>
    auto& inSocket(const SocketDecl& decl)
    {
        using T = SocketDecl::DataType;
        return std::get<LogicSocketInput<T>>(m_inSockets);
    }

    template<typename... Args>
    DummyNode(Args&&... args)
        : Base(std::forward<Args>(args)...)
    {
        Base::registerSockets(m_inSockets, m_outSockets);
    }

    void reset() override{};

    void compute() override
    {
    }

private:
    InSocketList m_inSockets;
    OutSocketList m_outSockets;
};

a.oSocket<>()

#endif