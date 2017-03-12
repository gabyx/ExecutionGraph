// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_nodes_LogicSocket_hpp
#define ExecutionGraph_nodes_LogicSocket_hpp

#include <meta/meta.hpp>

#include "ExecutionGraph/common/Asserts.hpp"
#include "ExecutionGraph/common/StaticAssert.hpp"
#include "ExecutionGraph/common/TypeDefs.hpp"
#include "ExecutionGraph/common/EnumClassHelper.hpp"
#include "ExecutionGraph/nodes/LogicCommon.hpp"


namespace ExecutionGraph
{
    
class LogicNode;
template <typename T> class LogicSocket;

/**
    The socket base class for all input/output sockets of a logic node. 
 */
template<typename Config>
class LogicSocketBase
{
public:

    EXEC_GRAPH_DEFINE_MATRIX_TYPES
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    using SocketListType = std::vector<std::unique_ptr<SocketBaseType>>
    
    //! Cast to a logic socket of type \p T.
    //! The cast fails at runtime if the type does not match!
    template <typename T>
    inline LogicSocket<T>* castToType()
    {
        EXEC_GRAPH_ASSERTMSG(this->m_type == meta::find_index<SocketTypes,T>,
                            "Types " << LogicTypes::getTypeName(this->m_type) << " and "
                                     << LogicTypes::getTypeName(iter::pos::value)
                            " of logic node '" << this->m_name << "' do not match!" 
                            );
        return static_cast<LogicSocket<T>*>(this);
    }
    
    //! Apply a visitor \p visitor on this logic socket.
    template <typename TVisitor>
    void applyVisitor(TVisitor&& visitor)
    {
        // implement switch statement in LogicTypes
        LOGICSOCKET_APPLY_VISITOR_SWITCH;
    }
    
    template<typename TTData>
    LogicSocketBase(unsigned int type, 
                    unsigned int id, 
                    NodeType* parent,
                    TTData&& defaultValue,
                    const std::string& name = "noname")
        : m_type(type),
        , m_id(id)
        , m_name(name)
        , m_parent(parent)
        , m_data(std::forward<TT>(defaultValue))
    {
    }

    virtual ~LogicSocketBase() = default;

    void link(LogicSocketBase* fsock);

    //! Get the parent logic node of this socket.
    inline NodeType* getParent() const
    {
        return m_parent;
    }

    //! Check if the socket has a Get-Link to an output socket.
    inline bool isConnectedToInput() const
    {
        return m_from != nullptr;
    }

    inline bool isConnected() const
    {
        return m_connected;
    }

    inline LogicSocketBase* getFrom() const
    {
        return m_from;
    }

public:
    const unsigned int m_type;      //!< The index in the mpl sequence, which type this is!
    const unsigned int m_id = 0;    //!< The unique id among all sockets of a LogicNode.  
    const std::string  m_name = ""; //!< The name of the socket.

protected:
    /** from socket to 'this' (used to link an input socket with an output socket)
    *  incoming edges: only one makes sense
    *  only valid for input sockets
    */
    LogicSocketBase* m_from = nullptr;

    /** from 'this' to sockets (used to link an output socket with one or more than one input socket)
    *   outgoing edges
    *   only valid for output sockets
    */
    SocketListType m_to;

private:
    NodeType* m_parent; //!< The parent logic node of of this socket.
    bool m_connected = false;

};

template <typename TData, typename TConfig>
class LogicSocketInput : public LogicSocketBase<TConfig>
{
public:

    using DataType = TData;
    /** This assert fails if the type T of the LogicSocket is 
        not properly added to the type list SocketTypes in LogicSocketBase*/
    EXEC_GRAPH_STATIC_ASSERT( !std::is_same< meta::find<SocketTypes,DataType>, meta::list<> >::value ,
                              "TData is not in SocketTypes!" )
                              
    template<typename... Args>
    LogicSocketInput(Args&&... args)
        : LogicSocketBase(std::forward<Args>(args)...)
    {
    }
    
private:
    TData m_data;  //!< Default value! or the output value if output socket
};

template <typename TData, typename TConfig>
class LogicSocketOutput: public LogicSocketBase<TConfig>
{
public:

    using DataType = TData;
    /** This assert fails if the type T of the LogicSocket is 
        not properly added to the type list SocketTypes in LogicSocketBase*/
    EXEC_GRAPH_STATIC_ASSERT( !std::is_same< meta::find<SocketTypes,DataType>, meta::list<> >::value ,
                              "TData is not in SocketTypes!" )
    
    template<typename... Args>
    LogicSocketOutput(Args&&... args)
        : LogicSocketBase(std::forward<Args>(args)...)
    {
    }
    
private:
    TData m_data;  //!< Default value! or the output value if output socket
};

#include "GRSF/logic/LogicNode.hpp"

template <typename T>
void LogicSocket<T>::distributeValue()
{
    if (!m_isInput)
    {
        // if output node
        // set all "to" sockets ( these are write links, because we write the value to another input socket)
        for (auto& s : m_to)
        {
            GRSF_ASSERTMSG(s->m_type == m_type,
                           "Types of node have to match: type " << m_type << "from id: " << s->getParent()->m_id
                                                                << "to type "
                                                                << s->m_type
                                                                << " of id: "
                                                                << s->getParent()->m_id)
            LogicSocket<T>* sock = static_cast<LogicSocket<T>*>(s);
            sock->setValue(m_data);
        }
    }
}

template <typename T>
template <typename TIn>
void LogicSocket<T>::setValue(const TIn& value)
{
    // set internal value for input node, however if output node we also set the value ( might be needed
    m_data = value;
    distributeValue();
};

template <typename T>
T LogicSocket<T>::getValue() const
{
    // if we have a "from" node (only for input sockets), we get the value from this sockets
    if (m_from)
    {
        return m_from->castToType<T>()->getValue();
    }
    return m_data;
};

template <typename T>
T& LogicSocket<T>::getValueRef()
{
    // if we have a from"
    if (m_from)
    {
        return m_from->castToType<T>()->getValueRef();
    }

    return m_data;
}

} // namespace

#define ADD_ISOCK(name, value)                                                                        \
    GRSF_ASSERTMSG(Inputs::name == this->m_inputs.size(), " Wrong order for Input: " << Inputs::name) \
    addISock<IType##name>(value);

#define ADD_OSOCK(name, value)                                                                            \
    GRSF_ASSERTMSG(Outputs::name == this->m_outputs.size(), " Wrong order for Output: " << Outputs::name) \
    addOSock<OType##name>(value);

#define REMOVEPTR_REMOVEREF(_ptr_) std::remove_pointer<typename std::remove_reference<decltype(_ptr_)>::type>::type

/** VALUE SETTER MACROS */
#define SET_ISOCKET_VALUE_PTR(ptr, name, value)                                                                    \
    ptr->setISocketValue<typename REMOVEPTR_REMOVEREF(ptr)::IType##name>((REMOVEPTR_REMOVEREF(ptr)::Inputs::name), \
                                                                         value)

#define SET_OSOCKET_VALUE_PTR(ptr, name, value)                                                                     \
    ptr->setOSocketValue<typename REMOVEPTR_REMOVEREF(ptr)::OType##name>((REMOVEPTR_REMOVEREF(ptr)::Outputs::name), \
                                                                         value)

#define DISTRIBUTE_OSOCKET_VALUE_PTR(ptr, name)                                  \
    ptr->distributeOSocketValue<typename REMOVEPTR_REMOVEREF(ptr)::OType##name>( \
        (REMOVEPTR_REMOVEREF(ptr)::Outputs::name))

#define SET_ISOCKET_VALUE(name, value) SET_ISOCKET_VALUE_PTR(this, name, value)

#define SET_OSOCKET_VALUE(name, value) SET_OSOCKET_VALUE_PTR(this, name, value)

#define DISTRIBUTE_OSOCKET_VALUE(name) DISTRIBUTE_OSOCKET_VALUE_PTR(this, name)

/** VALUE GETTER MACROS */
#define GET_ISOCKET_VALUE_PTR(ptr, name) \
    ptr->getISocketValue<typename REMOVEPTR_REMOVEREF(ptr)::IType##name>((REMOVEPTR_REMOVEREF(ptr)::Inputs::name))

#define GET_OSOCKET_VALUE_PTR(ptr, name) \
    ptr->getOSocketValue<typename REMOVEPTR_REMOVEREF(ptr)::OType##name>((REMOVEPTR_REMOVEREF(ptr)::Outputs::name))

#define GET_ISOCKET_VALUE(name) GET_ISOCKET_VALUE_PTR(this, name)

#define GET_OSOCKET_VALUE(name) GET_OSOCKET_VALUE_PTR(this, name)

/** VALUE GETTER REFERENCE MACROS */
#define GET_ISOCKET_REF_VALUE_PTR(ptr, name) getISocketRefValue<IType##name>((Inputs::name))

#define GET_OSOCKET_REF_VALUE_PTR(ptr, name) getOSocketRefValue<OType##name>((Outputs::name))

#define GET_ISOCKET_REF_VALUE(name) GET_ISOCKET_REF_VALUE_PTR(this, name)

#define GET_OSOCKET_REF_VALUE(name) GET_OSOCKET_REF_VALUE_PTR(this, name)

/** SOCKET GETTER REFERENCE MACROS */
#define GET_ISOCKET_PTR(ptr, name) getISocket<IType##name>((Inputs::name))

#define GET_OSOCKET_PTR(ptr, name) getOSocket<OType##name>((Outputs::name))

#define GET_ISOCKET(name) GET_ISOCKET_PTR(this, name)

#define GET_OSOCKET(name) GET_OSOCKET_PTR(this, name)

/** SOCKET DECLARATION MACROS */
#define DECLARE_ISOCKET_TYPE(name, type)         \
    typedef type IType##name;                    \
    inline LogicSocket<type>* getIn_##name()     \
    {                                            \
        return getISocket<type>((Inputs::name)); \
    }

#define DECLARE_OSOCKET_TYPE(name, type)          \
    typedef type OType##name;                     \
    inline LogicSocket<type>* getOut_##name()     \
    {                                             \
        return getOSocket<type>((Outputs::name)); \
    }

#endif