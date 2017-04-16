// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_nodes_LogicNode_hpp
#define ExecutionGraph_nodes_LogicNode_hpp

#include <memory>
#include <vector>
#include "ExecutionGraph/common/Asserts.hpp"
#include "ExecutionGraph/common/StaticAssert.hpp"
#include "ExecutionGraph/nodes/LogicCommon.hpp"

namespace executionGraph
{
/**
 @brief The execution node (logic node) class which is the base class
        for every logic node in an execution graph.

        // clang-format off
        General Concept
                               +-------------+                                       +-------------+
                               | LogicNode A |                                       | LogicNode B |
                               |             |                                       |             |
                               |      +------+-----+                           +-----+-----+       |
                               |      | Out Socket |                           | In Socket |       |
                               |      |            |                           |           |       |
                               |      |    m_to[0] |@------( Write-Link )----> |           |       |
                               |      |            |                           |           |       |
                               |      |            |                           |           |       |
                               |      |            |                           |           |       |
                               |      |            | <-----(  Get-Link  )-----@| m_from    |       |
                               |      |            |                           |           |       |
                               |      |            |                           |           |       |
                               |      |  T m_data  |                           |  T m_data |       |
                               |      +------+-----+                           +-----+-----+       |
                               |             |                                       |             |
                               +-------------+                                       +-------------+

                Function Behavior in Out Socket:  (this =  Out)  ++      Function Behavior in In Socket:  (this = In)
                ================================                 ||      ===============================
                                                                 ||
                -getValue():  gets this->m_data,                 ||      -getValue(): gets the value of the Get-Link
                                                                 ||                   Out->m_data
                                                                 ||
                                                                 ||
                -setValue():  set all Write-Links directly       ||      -setValue(): gets internal this->m_data
                              (m_to array) and set this->m_data  ||
                              also because this might have other ||
                              Get-Links                          ||
                                                                 ++
        // clang-format on
*/

template<typename TConfig>
class LogicNode
{
public:
    EXEC_GRAPH_TYPEDEF_CONFIG(TConfig);

    template<typename T>
    using SocketPointer        = std::unique_ptr<T, void (*)(T*)>;
    using SocketInputListType  = std::vector<SocketPointer<SocketInputBaseType>>;
    using SocketOutputListType = std::vector<SocketPointer<SocketOutputBaseType>>;

public:
    //! The basic constructor of a logic node.
    LogicNode(NodeId id): m_id(id) {}
    LogicNode(const LogicNode&) = default;
    LogicNode(LogicNode&&)      = default;

    virtual ~LogicNode() = default;

    //! The reset function.
    virtual void reset() = 0;

     //! The main compute function of this execution node.
    virtual void compute() = 0;

    inline NodeId getId(){ return m_id; }

    inline bool hasLinks() const { return m_hasLinks; }
    inline void setLinked(void) { m_hasLinks = true; }

//    //! Set the priority of this node to \p p.
//    inline void setPriority(unsigned int p) { m_priority = p; }
//    //! Get the priority of this node.
//    inline unsigned int getPriority(void) const { return m_priority; }
    //! Get the list of input sockets.
    const SocketInputListType& getInputs() const { return m_inputs; }
    //! Get the list of output sockets.
    const SocketOutputListType& getOutputs() const { return m_outputs; }

    //! Add an input socket with default value \p defaultValue.
    template<typename TData, typename T>
    void addISock(T&& defaultValue, const std::string& name = "noname")
    {
        SocketIndex id  = m_inputs.size();

        auto p = SocketPointer<SocketInputBaseType>(
            new SocketInputType<TData>(std::forward<T>(defaultValue), id, *this, name),
            [](SocketInputBaseType* p) { delete static_cast<SocketInputType<TData>*>(p); });
        m_inputs.push_back(std::move(p));
    }

    //! Add an input socket with default value \p defaultValue.
    template<typename TData, typename T>
    void addOSock(T&& defaultValue, const std::string& name = "noname")
    {
        SocketIndex id = m_outputs.size();

        auto p = SocketPointer<SocketOutputBaseType>(
            new SocketOutputType<TData>(std::forward<T>(defaultValue), id, *this, name),
            [](SocketOutputBaseType* p) { delete static_cast<SocketOutputType<TData>*>(p); });
        m_outputs.push_back(std::move(p));
    }

    //! Add all input sockets defined in the type list \p SocketDeclList where each socket has
    //! the corresponding default value in \p defaultValues.
    template<typename SocketDeclList,
             typename... Args,
             EXEC_GRAPH_SFINAE_ENABLE_IF((details::isInstantiationOf<details::InputSocketDeclarationList, SocketDeclList>::value))
            >
    void addSockets(std::tuple<Args...>&& defaultValues)
    {
        auto add = [&](auto socketDeclaration)
        {
            using SocketDeclaration = decltype(socketDeclaration);
            addISock<typename SocketDeclaration::DataType>(std::move(std::get<SocketDeclaration::Index::value>(defaultValues)));
        };

        meta::for_each( typename SocketDeclList::TypeList{} , add);
    }

    //! Add all output sockets defined in the type list \p SocketDeclList where each socket has
    //! the corresponding default value in \p defaultValues.
    template<typename SocketDeclList,
             typename... Args,
             EXEC_GRAPH_SFINAE_ENABLE_IF((details::isInstantiationOf<details::OutputSocketDeclarationList, SocketDeclList>::value))
            >
    void addSockets(std::tuple<Args...>&& defaultValues)
    {
        auto add = [&](auto socketDeclaration)
        {
            using SocketDeclaration = decltype(socketDeclaration);
            addOSock<typename SocketDeclaration::DataType>(std::move(std::get<SocketDeclaration::Index::value>(defaultValues)));
        };

        meta::for_each( typename SocketDeclList::TypeList{} , add);
    }

    //! Get the input socket at index \p idx.
    SocketInputBaseType& getISocket(SocketIndex idx)
    {
        EXEC_GRAPH_ASSERTMSG(idx < m_inputs.size(), "Wrong index!");
        return *m_inputs[idx];
    }
    //! Get the output socket at index \p index.
    SocketOutputBaseType& getOSocket(SocketIndex idx)
    {
        EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
        return *m_outputs[idx];
    }

    //! Get the input socket of type \p T at index \p idx.
    template<typename T>
    auto& getISocket(SocketIndex idx);
    template<typename T>
    const auto& getISocket(SocketIndex idx) const;

    //! Get the output socket of type \p T at index \p idx.
    template<typename T>
    auto& getOSocket(SocketIndex idx);
    template<typename T>
    const auto& getOSocket(SocketIndex idx) const;

    //! Get input socket value \p T at index \p idx.
    template<typename T>
    const T& getInVal(SocketIndex idx) const;

    //! Get the read/write output socket value \p T of the output socket at index \p idx.
    template<typename T>
    T& getOutVal(SocketIndex idx);
    template<typename T>
    const T& getOutVal(SocketIndex idx) const;

    //! Get the output socket value from a SocketDeclaration `OutputSocketDeclaration`.
    template<typename TSocketDeclaration, EXEC_GRAPH_SFINAE_ENABLE_IF((details::isInstantiationOf<details::OutputSocketDeclaration,TSocketDeclaration>::value)) >
    typename TSocketDeclaration::DataType& getValue();

    //! Get the output socket value from a SocketDeclaration `OutputSocketDeclaration`.
    template<typename TSocketDeclaration, EXEC_GRAPH_SFINAE_ENABLE_IF((details::isInstantiationOf<details::OutputSocketDeclaration,TSocketDeclaration>::value)) >
    const typename TSocketDeclaration::DataType& getValue() const;
    //! Get the input socket value from a SocketDeclaration `InputSocketDeclaration`.
    template<typename TSocketDeclaration, EXEC_GRAPH_SFINAE_ENABLE_IF((details::isInstantiationOf<details::InputSocketDeclaration,TSocketDeclaration>::value)) >
    const typename TSocketDeclaration::DataType& getValue() const;

    //! Writes the value of the output socket at index \p idx to all inputs
    //! which are accesible by write links.
    template<typename T>
    void distributeOSocketValue(SocketIndex idx);

    //! Constructs a Get-Link to get the data from output socket at index \p outS
    //! of logic node \p outN at the input socket at index \p inS of logic node \p
    //! inN.
    static void setGetLink(LogicNode& outN, SocketIndex outS, LogicNode& inN, SocketIndex inS);


    //! Constructs a Get-Link to get the data from output socket at index \p outS
    //! of logic node \p outN at the input socket at index \p inS.
    inline void setGetLink(LogicNode& outN, SocketIndex outS, SocketIndex inS)
    {
      setGetLink(outN, outS, *this, inS);
    }

    //! Constructs a Write-Link to write the data of output socket at index \p
    //! outS of logic node \p outN to the input socket at index \p inS of logic node \p
    //! inN.
    static void addWriteLink(LogicNode& outN, SocketIndex outS, LogicNode& inN, SocketIndex inS);

    //! Constructs a Write-Link to write the data of output socket at index \p
    //! outS
    //! of logic node \p outN to the input socket at index \p inS.
    inline void addWriteLink(IndexType outS, LogicNode& inN, SocketIndex inS)
    {
        addWriteLink(*this, outS, inN, inS);
    }

protected:
    const NodeId m_id;  //! The unique id of the logic node.
    bool m_hasLinks;
    SocketInputListType m_inputs;
    SocketOutputListType m_outputs;
//    unsigned int m_priority;
};

template<typename TConfig>
template<typename T>
auto& LogicNode<TConfig>::getISocket(SocketIndex idx)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_inputs.size(), "Wrong index!");
    return *m_inputs[idx]->template castToType<T>();
}

template<typename TConfig>
template<typename T>
const auto& LogicNode<TConfig>::getISocket(SocketIndex idx) const
{
    EXEC_GRAPH_ASSERTMSG(idx < m_inputs.size(), "Wrong index!");
    return *m_inputs[idx]->template castToType<T>();
}

template<typename TConfig>
template<typename T>
auto& LogicNode<TConfig>::getOSocket(SocketIndex idx)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    return *m_outputs[idx]->template castToType<T>();
}

template<typename TConfig>
template<typename T>
const auto& LogicNode<TConfig>::getOSocket(SocketIndex idx) const
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    return *m_outputs[idx]->template castToType<T>();
}

template<typename TConfig>
template<typename T>
const T& LogicNode<TConfig>::getInVal(SocketIndex idx) const
{
    EXEC_GRAPH_ASSERTMSG(idx < m_inputs.size(), "Wrong index!");
    return m_inputs[idx]->template castToType<T>()->getValue();
}

template<typename TConfig>
template<typename T>
T& LogicNode<TConfig>::getOutVal(SocketIndex idx)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    return m_outputs[idx]->template castToType<T>()->getValue();
}

template<typename TConfig>
template<typename T>
const T& LogicNode<TConfig>::getOutVal(SocketIndex idx) const
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    return m_outputs[idx]->template castToType<T>()->getValue();
}

template<typename TConfig>
template<typename TSocketDeclaration, EXEC_GRAPH_SFINAE_ENABLE_IMPL_IF((details::isInstantiationOf<details::OutputSocketDeclaration,TSocketDeclaration>::value))>
typename TSocketDeclaration::DataType& LogicNode<TConfig>::getValue()
{
    auto idx = TSocketDeclaration::Index::value;
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    return m_outputs[idx]->template castToType<typename TSocketDeclaration::DataType>()->getValue();
}

template<typename TConfig>
template<typename TSocketDeclaration, EXEC_GRAPH_SFINAE_ENABLE_IMPL_IF((details::isInstantiationOf<details::InputSocketDeclaration,TSocketDeclaration>::value))>
const typename TSocketDeclaration::DataType& LogicNode<TConfig>::getValue() const
{
    auto idx = TSocketDeclaration::Index::value;
    EXEC_GRAPH_ASSERTMSG(idx < m_inputs.size(), "Wrong index!");
    return m_inputs[idx]->template castToType<typename TSocketDeclaration::DataType>()->getValue();
}

template<typename TConfig>
template<typename TSocketDeclaration, EXEC_GRAPH_SFINAE_ENABLE_IMPL_IF((details::isInstantiationOf<details::OutputSocketDeclaration,TSocketDeclaration>::value))>
const typename TSocketDeclaration::DataType& LogicNode<TConfig>::getValue() const
{
    auto idx = TSocketDeclaration::Index::value;
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    return m_outputs[idx]->template castToType<typename TSocketDeclaration::DataType>()->getValue();
}

//template<typename TConfig>
//template<typename T, typename TIn>
//void LogicNode<TConfig>::setOSocketValue(IndexType idx, TIn&& data)
//{
//    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
//    m_outputs[idx]->template castToType<T>()->setValue(std::forward<TIn>(data));
//}

template<typename TConfig>
template<typename T>
void LogicNode<TConfig>::distributeOSocketValue(SocketIndex idx)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    m_outputs[idx]->template castToType<T>()->distributeValue();
}

template<typename TConfig>
void LogicNode<TConfig>::setGetLink(LogicNode& outN, SocketIndex outS, LogicNode& inN, SocketIndex inS)
{
    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(
        outS >= outN.getOutputs().size() || inS >= inN.getInputs().size(),
        "Wrong socket indices:  outNode: " << outN.getId()<< " outSocketIdx: " << outS << " inNode: " << inN.getId()
                                           << " inSocketIdx: "
                                           << inS, NodeConnectionException);

    inN.getISocket(inS).setGetLink(outN.getOSocket(outS));
}

template<typename TConfig>
void LogicNode<TConfig>::addWriteLink(LogicNode& outN, SocketIndex outS, LogicNode& inN, SocketIndex inS)
{
    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(
        outS >= outN.getOutputs().size() || inS >= inN.getInputs().size(),
        "Wrong socket indices:  outNode: " << outN.getId()<< " outSocketIdx: " << outS << " inNode: " << inN.getId()
                                           << " inSocketIdx: "
                                           << inS << "(nOuts: " << outN.getOutputs().size() << ", nIns: " << inN.getInputs().size() << ")"
                                      , NodeConnectionException);

    outN.getOSocket(outS).addWriteLink(inN.getISocket(inS));
}


//! Some handy macros to redefine getters to shortcut the following ugly syntax inside a derivation of LogicNode:
//! Accessing the value in socket Result1 : this->template getValue<typename OutSockets::template Get<Result1>>();
#define EXEC_GRAPH_DEFINE_VALUE_GETTERS(InputEnum, InSocketDeclList, OutputEnum, OutSocketDeclList)                         \
    template<InputEnum S>                                                                                                   \
    inline auto& getInVal() const{ return this->template getValue<typename InSocketDeclList::template Get<S>>(); }          \
                                                                                                                            \
    template<OutputEnum S>                                                                                                  \
    inline auto& getOutVal(){ return this->template getValue<typename OutSocketDeclList::template Get<S>>(); }              \
                                                                                                                            \
    template<OutputEnum S>                                                                                                  \
    inline auto& getInVal() const{ return this->template getValue<typename OutSocketDeclList::template Get<S>>(); }



}  // end ExecutionGraph

#endif
