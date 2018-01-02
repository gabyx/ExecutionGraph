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
#include "ExecutionGraph/common/DemangleTypes.hpp"
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
                               |     +-------+-----+                           +-----+-----+       |
                               |     |  Out Socket |                           | In Socket |       |
                               |     |             |                           |           |       |
                               |     | m_writeTo[0]|@------( Write-Link )----> |           |       |
                               |     |             |                           |           |       |
                               |     |             |                           |           |       |
                               |     |             |                           |           |       |
                               |     |             | <-----(  Get-Link  )-----@| m_getFrom |       |
                               |     |             |                           |           |       |
                               |     |             |                           |           |       |
                               |     |   T m_data  |                           | T* m_data |       |
                               |     +-------+-----+                           +-----+-----+       |
                               |             |                                       |             |
                               +-------------+                                       +-------------+

                Function Behavior in Out Socket:  (this =  Out)  ++      Function Behavior in In Socket:  (this = In)
                ================================                 ||      ===============================
                                                                 ||
                -getValue():  gets this->m_data,                 ||      -getValue(): gets the value *this->m_data
                                                                 ||                   Exception if nullptr
                                                                 ||
                                                                 ||
                -setValue():  set all Write-Links directly       ||      -setValue(): sets internal this->m_data
                              (m_writeTo array) and set          ||
                              this->m_data also because this     ||
                              might have other                   ||
                              Get-Links                          ||
                                                                 ++
        // clang-format on
*/

template<typename TConfig>
class LogicNode
{
public:
    EXECGRAPH_TYPEDEF_CONFIG(TConfig);

    using SocketInputListType  = std::vector<SocketInputBasePointer>;
    using SocketOutputListType = std::vector<SocketOutputBasePointer>;

public:
    //! The basic constructor of a logic node.
    LogicNode(NodeId id, std::string name = "")
        : m_id(id), m_name(name.empty() ? std::to_string(id) : name) {}
    LogicNode(const LogicNode&) = default;
    LogicNode(LogicNode&&)      = default;

    virtual ~LogicNode()
    {
        EXECGRAPH_LOG_TRACE("Destructor: LogicNode: " << m_name);
    }

    //! The reset function.
    virtual void reset() = 0;

    //! The main compute function of this execution node.
    virtual void compute() = 0;

    inline NodeId getId() const { return m_id; }
    inline std::string getName() const { return m_name; }

    //! Get the list of input sockets.
    const SocketInputListType& getInputs() const { return m_inputs; }
    SocketInputListType& getInputs() { return m_inputs; }
    //! Get the list of output sockets.
    const SocketOutputListType& getOutputs() const { return m_outputs; }
    SocketOutputListType& getOutputs() { return m_outputs; }

    //! Get the number of input sockets which are connected to other nodes.
    IndexType getConnectedInputCount() const;

    //! Get the number of output sockets which are connected to other nodes.
    IndexType getConnectedOutputCount() const;

    //! Add an input socket with default value from the default output socket \p defaultOutputSocketId.
    template<typename TData>
    void addISock(IndexType defaultOutputSocketId = meta::find_index<SocketTypes, TData>::value,
                  const std::string& name         = "noname")
    {
        SocketIndex id = m_inputs.size();

        auto p = SocketPointer<SocketInputBaseType>(
            new SocketInputType<TData>(defaultOutputSocketId, id, *this, name),
            [](SocketInputBaseType* s) { delete static_cast<SocketInputType<TData>*>(s); });
        m_inputs.push_back(std::move(p));
    }

    //! Add an input socket with default value \p defaultValue.
    template<typename TData, typename T>
    void addOSock(T&& defaultValue,
                  const std::string& name = "noname")
    {
        SocketIndex id = m_outputs.size();

        auto p = SocketPointer<SocketOutputBaseType>(
            new SocketOutputType<TData>(std::forward<T>(defaultValue), id, *this, name),
            [](SocketOutputBaseType* s) { delete static_cast<SocketOutputType<TData>*>(s); });
        m_outputs.push_back(std::move(p));
    }

    //! Add all input sockets defined in the type list \p SocketDeclList.
    template<typename SocketDeclList,
             EXECGRAPH_SFINAE_ENABLE_IF((details::isInstantiationOf<details::InputSocketDeclarationList, SocketDeclList>::value))>
    void addSockets()
    {
        auto add = [&](auto socketDeclaration) {
            using SocketDeclaration = decltype(socketDeclaration);
            this->template addISock<typename SocketDeclaration::DataType>();
        };

        meta::for_each(typename SocketDeclList::TypeList{}, add);
    }

    //! Add all output sockets defined in the type list \p SocketDeclList where each socket has
    //! the corresponding default value in \p defaultValues.
    template<typename SocketDeclList,
             typename... Args,
             EXECGRAPH_SFINAE_ENABLE_IF((details::isInstantiationOf<details::OutputSocketDeclarationList, SocketDeclList>::value))>
    void addSockets(std::tuple<Args...>&& defaultValues)
    {
        auto add = [&](auto socketDeclaration) {
            using SocketDeclaration = decltype(socketDeclaration);
            this->template addOSock<typename SocketDeclaration::DataType>(std::move(std::get<SocketDeclaration::Index::value>(defaultValues)));
        };

        meta::for_each(typename SocketDeclList::TypeList{}, add);
    }

    bool hasISocket(SocketIndex idx) { return idx < m_inputs.size(); }
    bool hasOSocket(SocketIndex idx) { return idx < m_outputs.size(); }

    //! Get the input socket at index \p idx.
    SocketInputBaseType& getISocket(SocketIndex idx)
    {
        EXECGRAPH_ASSERT(idx < m_inputs.size(), "Wrong index!");
        return *m_inputs[idx];
    }
    //! Get the output socket at index \p index.
    SocketOutputBaseType& getOSocket(SocketIndex idx)
    {
        EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
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
    template<typename TSocketDeclaration, EXECGRAPH_SFINAE_ENABLE_IF((details::isInstantiationOf<details::OutputSocketDeclaration, TSocketDeclaration>::value))>
    typename TSocketDeclaration::DataType& getValue();
    //! Get the output socket value from a SocketDeclaration `OutputSocketDeclaration`.
    template<typename TSocketDeclaration, EXECGRAPH_SFINAE_ENABLE_IF((details::isInstantiationOf<details::OutputSocketDeclaration, TSocketDeclaration>::value))>
    const typename TSocketDeclaration::DataType& getValue() const;

    //! Get the input socket value from a SocketDeclaration `InputSocketDeclaration`.
    template<typename TSocketDeclaration, EXECGRAPH_SFINAE_ENABLE_IF((details::isInstantiationOf<details::InputSocketDeclaration, TSocketDeclaration>::value))>
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

    virtual std::string getTypeName() { return shortenTemplateBrackets(demangle(this)); }

protected:
    const NodeId m_id;   //! The unique id of the logic node.
    std::string m_name;  //! The name of the logic node.
    SocketInputListType m_inputs;
    SocketOutputListType m_outputs;
};

template<typename TConfig>
IndexType LogicNode<TConfig>::getConnectedInputCount() const
{
    IndexType count = 0;
    for(auto& socket : this->getInputs())
    {
        if(socket->getConnectionCount() > 0)
        {
            ++count;
        }
    }
    return count;
}

template<typename TConfig>
IndexType LogicNode<TConfig>::getConnectedOutputCount() const
{
    IndexType count = 0;
    for(auto& socket : this->getOutputs())
    {
        if(socket->getConnectionCount() > 0)
        {
            ++count;
        }
    }
    return count;
}

template<typename TConfig>
template<typename T>
auto& LogicNode<TConfig>::getISocket(SocketIndex idx)
{
    EXECGRAPH_ASSERT(idx < m_inputs.size(), "Wrong index!");
    return *m_inputs[idx]->template castToType<T>();
}

template<typename TConfig>
template<typename T>
const auto& LogicNode<TConfig>::getISocket(SocketIndex idx) const
{
    EXECGRAPH_ASSERT(idx < m_inputs.size(), "Wrong index!");
    return *m_inputs[idx]->template castToType<T>();
}

template<typename TConfig>
template<typename T>
auto& LogicNode<TConfig>::getOSocket(SocketIndex idx)
{
    EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    return *m_outputs[idx]->template castToType<T>();
}

template<typename TConfig>
template<typename T>
const auto& LogicNode<TConfig>::getOSocket(SocketIndex idx) const
{
    EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    return *m_outputs[idx]->template castToType<T>();
}

template<typename TConfig>
template<typename T>
const T& LogicNode<TConfig>::getInVal(SocketIndex idx) const
{
    EXECGRAPH_ASSERT(idx < m_inputs.size(), "Wrong index!");
    return m_inputs[idx]->template castToType<T>()->getValue();
}

template<typename TConfig>
template<typename T>
T& LogicNode<TConfig>::getOutVal(SocketIndex idx)
{
    EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    return m_outputs[idx]->template castToType<T>()->getValue();
}

template<typename TConfig>
template<typename T>
const T& LogicNode<TConfig>::getOutVal(SocketIndex idx) const
{
    EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    return m_outputs[idx]->template castToType<T>()->getValue();
}

template<typename TConfig>
template<typename TSocketDeclaration, EXECGRAPH_SFINAE_ENABLE_IMPL_IF((details::isInstantiationOf<details::OutputSocketDeclaration, TSocketDeclaration>::value))>
typename TSocketDeclaration::DataType& LogicNode<TConfig>::getValue()
{
    auto idx = TSocketDeclaration::Index::value;
    EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    return m_outputs[idx]->template castToType<typename TSocketDeclaration::DataType>()->getValue();
}

template<typename TConfig>
template<typename TSocketDeclaration, EXECGRAPH_SFINAE_ENABLE_IMPL_IF((details::isInstantiationOf<details::InputSocketDeclaration, TSocketDeclaration>::value))>
const typename TSocketDeclaration::DataType& LogicNode<TConfig>::getValue() const
{
    auto idx = TSocketDeclaration::Index::value;
    EXECGRAPH_ASSERT(idx < m_inputs.size(), "Wrong index!");
    return m_inputs[idx]->template castToType<typename TSocketDeclaration::DataType>()->getValue();
}

template<typename TConfig>
template<typename TSocketDeclaration, EXECGRAPH_SFINAE_ENABLE_IMPL_IF((details::isInstantiationOf<details::OutputSocketDeclaration, TSocketDeclaration>::value))>
const typename TSocketDeclaration::DataType& LogicNode<TConfig>::getValue() const
{
    auto idx = TSocketDeclaration::Index::value;
    EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    return m_outputs[idx]->template castToType<typename TSocketDeclaration::DataType>()->getValue();
}

template<typename TConfig>
template<typename T>
void LogicNode<TConfig>::distributeOSocketValue(SocketIndex idx)
{
    EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    m_outputs[idx]->template castToType<T>()->distributeValue();
}

template<typename TConfig>
void LogicNode<TConfig>::setGetLink(LogicNode& outN, SocketIndex outS, LogicNode& inN, SocketIndex inS)
{
    EXECGRAPH_THROW_EXCEPTION_TYPE_IF(
        outS >= outN.getOutputs().size() || inS >= inN.getInputs().size(),
        "Wrong socket indices:  outNode: " << outN.getId() << " outSocketIdx: " << outS << " inNode: " << inN.getId()
                                           << " inSocketIdx: "
                                           << inS,
        NodeConnectionException);

    inN.getISocket(inS).setGetLink(outN.getOSocket(outS));
}

template<typename TConfig>
void LogicNode<TConfig>::addWriteLink(LogicNode& outN, SocketIndex outS, LogicNode& inN, SocketIndex inS)
{
    EXECGRAPH_THROW_EXCEPTION_TYPE_IF(
        outS >= outN.getOutputs().size() || inS >= inN.getInputs().size(),
        "Wrong socket indices:  outNode: " << outN.getId() << " outSocketIdx: " << outS << " inNode: " << inN.getId()
                                           << " inSocketIdx: "
                                           << inS
                                           << " (nOuts: " << outN.getOutputs().size() << ", nIns: " << inN.getInputs().size() << ")",
        NodeConnectionException);

    outN.getOSocket(outS).addWriteLink(inN.getISocket(inS));
}

//! Some handy macros to redefine getters to shortcut the following ugly syntax inside a derivation of LogicNode:
//! Accessing the value in socket Result1 : this->template getValue<typename OutSockets::template Get<Result1>>();
#define EXECGRAPH_DEFINE_LOGIC_NODE_VALUE_GETTERS(InputEnum, InSocketDeclList, OutputEnum, OutSocketDeclList)        \
    template<InputEnum S>                                                                                            \
    inline auto& getInVal() const { return this->template getValue<typename InSocketDeclList::template Get<S>>(); }  \
                                                                                                                     \
    template<OutputEnum S>                                                                                           \
    inline auto& getOutVal() { return this->template getValue<typename OutSocketDeclList::template Get<S>>(); }      \
                                                                                                                     \
    template<OutputEnum S>                                                                                           \
    inline auto& getInVal() const { return this->template getValue<typename OutSocketDeclList::template Get<S>>(); } \
                                                                                                                     \
    template<InputEnum S>                                                                                            \
    static constexpr const SocketIndex& getInIdx() { return InSocketDeclList::template Get<S>::Index::value; }       \
    template<OutputEnum S>                                                                                           \
    static constexpr const SocketIndex& getOutIdx() { return OutSocketDeclList::template Get<S>::Index::value; }

#define EXECGRAPH_DEFINE_LOGIC_NODE_GET_TYPENAME() \
    virtual std::string getTypeName() override { return shortenTemplateBrackets(demangle(this)); };

}  // end ExecutionGraph

#endif
