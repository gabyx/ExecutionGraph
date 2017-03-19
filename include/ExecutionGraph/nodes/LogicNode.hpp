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

namespace ExecutionGraph
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
    EXEC_GRAPH_TYPDEF_CONFIG(TConfig);

    template<typename T>
    using SocketPointer        = std::unique_ptr<T, void (*)(T*)>;
    using SocketInputListType  = std::vector<SocketPointer<SocketInputBaseType>>;
    using SocketOutputListType = std::vector<SocketPointer<SocketOutputBaseType>>;

public:
    //! The basic constructor of a logic node.
    LogicNode(unsigned int id): m_id(id) {}
    LogicNode(const LogicNode&) = default;
    LogicNode(LogicNode&&)      = default;

    virtual ~LogicNode() = default;

    //! The reset function.
    virtual void reset() = 0;

     //! The main compute function of this execution node.
    virtual void compute() = 0;

    inline unsigned int getId(){ return m_id; }

    inline bool hasLinks() const { return m_hasLinks; }
    inline void setLinked(void) { m_hasLinks = true; }

    //! Set the priority of this node to \p p.
    inline void setPriority(unsigned int p) { m_priority = p; }
    //! Get the priority of this node.
    inline unsigned int getPriority(void) const { return m_priority; }
    //! Get the list of input sockets.
    const SocketInputListType& getInputs() const { return m_inputs; }
    //! Get the list of output sockets.
    const SocketOutputListType& getOutputs() const { return m_outputs; }

    //! Add an input socket with default value \p defaultValue.
    template<typename TData, typename T>
    void addISock(T&& defaultValue, const std::string& name = "noname")
    {
        unsigned int id = m_inputs.size() + m_outputs.size();
        auto p           = SocketPointer<SocketInputBaseType>(
            new SocketInputType<TData>(std::forward<T>(defaultValue), id, *this, name),
            [](SocketInputBaseType* p) { delete static_cast<SocketInputType<TData>*>(p); });
        m_inputs.push_back(std::move(p));
    }

    //! Add an input socket with default value \p defaultValue.
    template<typename Enum, typename TData, typename T>
    void addISock(T&& defaultValue, const std::string& name = "noname")
    {
        unsigned int id = m_inputs.size() + m_outputs.size();
        auto p           = SocketPointer<SocketInputBaseType>(
            new SocketInputType<TData>(std::forward<T>(defaultValue), id, *this, name),
            [](SocketInputBaseType* p) { delete static_cast<SocketInputType<TData>*>(p); });
        m_inputs.push_back(std::move(p));
    }

    //! Add an input socket with default value \p defaultValue.
    template<typename TData, typename T>
    void addOSock(T&& defaultValue, const std::string& name = "noname")
    {
        unsigned int idx = m_inputs.size() + m_outputs.size();
        auto p           = SocketPointer<SocketOutputBaseType>(
            new SocketOutputType<TData>(std::forward<T>(defaultValue), idx, *this, name),
            [](SocketOutputBaseType* p) { delete static_cast<SocketOutputType<TData>*>(p); });
        m_outputs.push_back(std::move(p));
    }

    //! Get the input socket at index \p idx.
    SocketInputBaseType& getISocket(unsigned int idx)
    {
        EXEC_GRAPH_ASSERTMSG(idx < m_inputs.size(), "Wrong index!");
        return *m_inputs[idx];
    }
    //! Get the output socket at index \p index.
    SocketOutputBaseType& getOSocket(unsigned int idx)
    {
        EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
        return *m_outputs[idx];
    }

    //! Get the input socket of type \p T at index \p idx .
    template<typename T>
    auto& getISocket(unsigned int idx);
    template<typename T>
    const auto& getISocket(unsigned int idx) const;

    //! Get the output socket of type \p T at index \p idx.
    template<typename T>
    auto& getOSocket(unsigned int idx);
    template<typename T>
    const auto& getOSocket(unsigned int idx) const;

    //! Get input socket value \p T at index \p idx.
    template<typename T>
    const T& getISocketValue(unsigned int idx) const;

    //! Get the read/write socket value \p T of the output socket at index \p idx.
    template<typename T>
    T& getOSocketValue(unsigned int idx);
    template<typename T>
    const T& getOSocketValue(unsigned int idx) const;

    //! Set the read/write output socket value \p T at index \p idx.
    template<typename T, typename TIn = T>
    void setOSocketValue(unsigned int idx, TIn&& data);

    //! Writes the value of the output socket at index \p idx to all inputs
    //! which are accesible by write links.
    template<typename T>
    void distributeOSocketValue(unsigned int idx);

    //! Constructs a Get-Link to get the data from output socket at index \p outS
    //! of logic node \p outN at the input socket at index \p inS of logic node \p
    //! inN.
    static void setGetLink(LogicNode& outN, unsigned int outS, LogicNode& inN, unsigned int inS);


    //! Constructs a Get-Link to get the data from output socket at index \p outS
    //! of logic node \p outN at the input socket at index \p inS.
    inline void setGetLink(LogicNode& outN, unsigned int outS, unsigned int inS)
    {
      setGetLink(outN, outS, *this, inS);
    }

    //! Constructs a Write-Link to write the data of output socket at index \p
    //! outS of logic node \p outN to the input socket at index \p inS of logic node \p
    //! inN.
    static void addWriteLink(LogicNode& outN, unsigned int outS, LogicNode& inN, unsigned int inS);

    //! Constructs a Write-Link to write the data of output socket at index \p
    //! outS
    //! of logic node \p outN to the input socket at index \p inS.
    inline void addWriteLink(unsigned int outS, LogicNode& inN, unsigned int inS)
    {
        addWriteLink(*this, outS, inN, inS);
    }

protected:
    const unsigned int m_id;  //! The unique id of the logic node.
    bool m_hasLinks;
    SocketInputListType m_inputs;
    SocketOutputListType m_outputs;
    unsigned int m_priority;
};

template<typename TConfig>
template<typename T>
auto& LogicNode<TConfig>::getISocket(unsigned int idx)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_inputs.size(), "Wrong index!");
    return *m_inputs[idx]->template castToType<T>();
}

template<typename TConfig>
template<typename T>
const auto& LogicNode<TConfig>::getISocket(unsigned int idx) const
{
    EXEC_GRAPH_ASSERTMSG(idx < m_inputs.size(), "Wrong index!");
    return *m_inputs[idx]->template castToType<T>();
}

template<typename TConfig>
template<typename T>
auto& LogicNode<TConfig>::getOSocket(unsigned int idx)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    return *m_outputs[idx]->template castToType<T>();
}

template<typename TConfig>
template<typename T>
const auto& LogicNode<TConfig>::getOSocket(unsigned int idx) const
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    return *m_outputs[idx]->template castToType<T>();
}

template<typename TConfig>
template<typename T>
const T& LogicNode<TConfig>::getISocketValue(unsigned int idx) const
{
    EXEC_GRAPH_ASSERTMSG(idx < m_inputs.size(), "Wrong index!");
    return m_inputs[idx]->template castToType<T>()->getValue();
}

template<typename TConfig>
template<typename T>
T& LogicNode<TConfig>::getOSocketValue(unsigned int idx)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    return m_outputs[idx]->template castToType<T>()->getValue();
}

template<typename TConfig>
template<typename T>
const T& LogicNode<TConfig>::getOSocketValue(unsigned int idx) const
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    return m_outputs[idx]->template castToType<T>()->getValue();
}

template<typename TConfig>
template<typename T, typename TIn>
void LogicNode<TConfig>::setOSocketValue(unsigned int idx, TIn&& data)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    m_outputs[idx]->template castToType<T>()->setValue(std::forward<TIn>(data));
}

template<typename TConfig>
template<typename T>
void LogicNode<TConfig>::distributeOSocketValue(unsigned int idx)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    m_outputs[idx]->template castToType<T>()->distributeValue();
}

template<typename TConfig>
void LogicNode<TConfig>::setGetLink(LogicNode& outN, unsigned int outS, LogicNode& inN, unsigned int inS)
{
    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(
        outS >= outN.getOutputs().size() || inS >= inN.getInputs().size(),
        "Wrong socket indices:  outNode: " << outN.getId()<< " outSocketIdx: " << outS << " inNode: " << inN.getId()
                                           << " inSocketIdx: "
                                           << inS, NodeConnectionException);

    inN.getISocket(inS).setGetLink(outN.getOSocket(outS));
}

template<typename TConfig>
void LogicNode<TConfig>::addWriteLink(LogicNode& outN, unsigned int outS, LogicNode& inN, unsigned int inS)
{
    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(
        outS >= outN.getOutputs().size() || inS >= inN.getInputs().size(),
        "Wrong socket indices:  outNode: " << outN.getId()<< " outSocketIdx: " << outS << " inNode: " << inN.getId()
                                           << " inSocketIdx: "
                                           << inS << "(nOuts: " << outN.getOutputs().size() << ", nIns: " << inN.getInputs().size() << ")"
                                      , NodeConnectionException);

    outN.getOSocket(outS).addWriteLink(inN.getISocket(inS));
}


//! Some handy macro to use when inheriting from LogicNode.
#define EXEC_GRAPH_DEFINE_SOCKET_INFO   \
    enum class SocketInfos : uint64_t   \
    {                                   \
        nInputs  = Inputs::InputLast,   \
        nOutputs = Outputs::OutputLast, \
        nSockets = nInputs + nOutputs,  \
    };

}  // namespace

#endif
