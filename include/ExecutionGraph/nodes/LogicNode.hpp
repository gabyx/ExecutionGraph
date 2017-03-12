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

#include <vector>
#include <memory>
#include "ExecutionGraph/common/Asserts.hpp"
#include "ExecutionGraph/common/LogicCommon.hpp"

namespace ExecutionGraph
{

class LogicSocketBase;
template <typename T> class LogicSocket;

/**
 @brief The execution node (logic node) class which is the base class 
        for every logic node in an execution graph. 
  
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
*/

template<typename TSocketTypes>
struct GeneralConfig
{
    using SocketTypes = TSocketTypes;
    using NodeBaseType                           = LogicNode<Config>;
    using SocketBaseType                         = LogicSocketBase<Config>;
    template<typename T> using SocketInputType   = LogicSocketInput<T,Config>;
    template<typename T> using SocketOutputType  = LogicSocketOutput<T,Config>;
}

#define EXEC_GRAPH_TYPDEF_CONFIG(__CONFIG__) \
    using Config = __CONFIG__; \
    using SocketTypes = __CONFIG__::SocketTypes; \
    using NodeBaseType                            = typename Config::NodeBaseType; \
    using SocketBaseType                          = typename Config::SocketBaseType; \
    template<typename T> using SocketInputType    = typename Config::template SocketInputType<T>; \
    template<typename T> using SocketOutputType   = typename Config::template SocketOutputType<T>

template< typename TConfig >
class LogicNode
{
public:
    
    EXEC_GRAPH_TYPDEF_CONFIG(TConfig);
    
    using SocketListType = std::vector<std::unique_ptr<SocketBaseType>>
    
    const unsigned int m_id; //! The unique id of the logic node.

public:
    
    //! The basic constructor of a logic node.
    LogicNode(unsigned int id);
    LogicNode(const LogicNode&) = default;
    LogicNode(LogicNode&&) = default;
    
    virtual ~LogicNode();

    //! The reset function.
    virtual void reset() = 0;

    //! The main compute function of this execution node.
    virtual void compute() = 0;

    //! Get the input socket at index \p idx.
    SocketBaseType* getISocket(unsigned int idx);
    //! Get the output socket at index \p index.
    SocketBaseType* getOSocket(unsigned int idx);

    inline bool hasLinks() const
    {
        return m_hasLinks;
    }
    inline void setLinked(void)
    {
        m_hasLinks = true;
    }
    
    //! Set the priority of this node to \p p.
    inline void setPriority(unsigned int p)
    {
        m_priority = p;
    }
    //! Get the priority of this node.
    inline unsigned int getPriority(void) const
    {
        return m_priority;
    }
    //! Get the list of input sockets.
    const SocketListType& getInputs() const
    {
        return m_inputs;
    }
    //! Get the list of output sockets.
    const SocketListType& getOutputs() const
    {
        return m_outputs;
    }
    
    //! Add an input socket with default value \p defaultValue.
    template <typename T>
    void addISock(T&& defaultValue, const std::string& name = "noname")
    {
        EXEC_GRAPH_STATIC_ASSERTM(!std::is_reference<T>::value,"Wrong template argument");
        
        unsigned int idx = m_inputs.size() + m_outputs.size();
        auto p = std::make_unique<LogicSocketOutput<T>>(this, std::forward<T>(defaultValue), idx, name));
        m_inputs.push_back(std::move(p));
    }
    
    //! Add an input socket with default value \p defaultValue.
    template <typename T>
    void addISock(T&& defaultValue, const std::string& name = "noname")
    {
        EXEC_GRAPH_STATIC_ASSERTM(!std::is_reference<T>::value,"Wrong template argument");
        
        unsigned int idx = m_inputs.size() + m_outputs.size();
        auto p = std::make_unique<LogicSocketInput<T>>(this, std::forward<T>(defaultValue), idx, name));
        m_inputs.push_back(std::move(p));
    }
    
    //! Get the input socket of type \p T at index \p idx .
    template <typename T> 
    SocketInputType<T>* getISocket(unsigned int idx);
    template <typename T> 
    const SocketInputType<T> * getISocket(unsigned int idx) const;
    
    //! Get the output socket of type \p T at index \p idx.
    template <typename T> 
    SocketOutputType<T>* getOSocket(unsigned int idx);
    template <typename T> 
    const SocketOutputType<T> * getOSocket(unsigned int idx) const;

    //! Get input socket value \p T at index \p idx.
    template <typename T> 
    const& T getISocketValue(unsigned int idx) const;
    //! Set the input socket value \p T at index \p idx.
    template <typename T, typename TIn = T>
    void setISocketValue(unsigned int idx, const TIn& data);
    
    //! Get the read/write socket value \p T of the output socket at index \p idx.
    template <typename T>
    T& getOSocketValue(unsigned int idx);
    template <typename T>
    const T& getOSocketValue(unsigned int idx) const;
    
    //! Set the read/write output socket value \p T at index \p idx.
    template <typename T, typename TIn = T>
    void setOSocketValue(unsigned int idx, const TIn& data);
    
    //! Writes the value of the output socket at index \p idx to all inputs 
    //! which are accesible by write links. 
    template <typename T>
    void distributeOSocketValue(unsigned int idx); 
    
    //! Constructs a Get-Link to get the data from output socket at index \p outS  
    //! of logic node \p outN at the input socket at index \p inS of logic node \p inN.
    static void makeGetLink(LogicNode* outN, unsigned int outS, LogicNode* inN, unsigned int inS);
    
    //! Constructs a Get-Link to get the data from output socket at index \p outS  
    //! of logic node \p outN at the input socket at index \p inS.
    inline void makeGetLink(LogicNode* outN, unsigned int outS, unsigned int inS)
    {
        makeGetLink(outN,outS,this,inS);
    }

    //! Constructs a Write-Link to write the data of output socket at index \p outS  
    //! of logic node \p outN to the input socket at index \p inS of logic node \p inN.
    static void makeWriteLink(LogicNode* outN, unsigned int outS, LogicNode* inN, unsigned int inS);
    
    //! Constructs a Write-Link to write the data of output socket at index \p outS  
    //! of logic node \p outN to the input socket at index \p inS.
    inline void makeWriteLink(LogicNode* outN, unsigned int outS, unsigned int inS)
    {
        makeWriteLink(outN,outS,this,inS);
    }

protected:
    bool m_hasLinks;
    SocketListType m_inputs;
    SocketListType m_outputs;
    unsigned int m_priority;
};

template <typename T>
LogicSocket<T>* LogicNode::getISocket(unsigned int idx)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_inputs.size(), "Wrong index!");
    return m_inputs[idx]->castToType<T>();
}

template <typename T>
LogicSocket<T>* LogicNode::getOSocket(unsigned int idx)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    return m_outputs[idx]->castToType<T>();
}

template <typename T>
const T& LogicNode::getISocketValue(unsigned int idx) const
{
    EXEC_GRAPH_ASSERTMSG(idx < m_inputs.size(), "Wrong index!");
    return m_inputs[idx]->castToType<T>()->getValue();
}

template <typename T>
T LogicNode::getOSocketValue(unsigned int idx)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size(), "Wrong index!");
    return m_outputs[idx]->castToType<T>()->getValue();
}

template <typename T>
const& T LogicNode::getOSocketValue(unsigned int idx) const
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size() , "Wrong index!");
    return m_outputs[idx]->castToType<T>()->getValue();
}

template <typename T, typename TIn>
void LogicNode::setISocketValue(unsigned int idx, const TIn& data)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_inputs.size() , "Wrong index!");
    m_inputs[idx]->castToType<T>()->setValue(data);
}
template <typename T, typename TIn>
void LogicNode::setOSocketValue(unsigned int idx, const TIn& data)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size() , "Wrong index!");
    m_outputs[idx]->castToType<T>()->setValue(data);
}

template <typename T>
void LogicNode::distributeOSocketValue(unsigned int idx)
{
    EXEC_GRAPH_ASSERTMSG(idx < m_outputs.size() , "Wrong index!");
    m_outputs[idx]->castToType<T>()->distributeValue();
}

//! Some handy macro to use when inheriting from LogicNode.
#define GRSF_LN_DECLARE_SIZES       \
enum class SocketInfos : uint64_t   \
{                                   \
    nInputs  = Inputs::InputLast,   \
    nOutputs = Outputs::OutputLast, \
    nSockets = nInputs + nOutputs,  \
};

} // namespace

#endif