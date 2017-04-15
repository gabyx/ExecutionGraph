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
#include "ExecutionGraph/common/EnumClassHelper.hpp"
#include "ExecutionGraph/common/StaticAssert.hpp"
#include "ExecutionGraph/common/TypeDefs.hpp"
#include "ExecutionGraph/nodes/LogicCommon.hpp"

namespace executionGraph
{
//! The socket base class for all input/output sockets of a logic node.
template<typename TConfig>
class LogicSocketBase
{
public:
    EXEC_GRAPH_TYPEDEF_CONFIG(TConfig);

    LogicSocketBase(unsigned int type, SocketId id, NodeBaseType& parent, const std::string& name = "noname")
        : m_type(type), m_id(id), m_parent(parent),  m_name(name)
    {}

    unsigned int getType() const { return m_type; }
    SocketId getId() const { return m_id; }
    std::string getName() const { return m_name; }

    const NodeBaseType& getParent() const { return m_parent; }
    NodeBaseType& getParent() { return m_parent; }

protected:
    const unsigned int m_type;     //!< The index in the mpl sequence, which type this is!
    const SocketId m_id;       //!< The unique id among all sockets of a LogicNode.
    NodeBaseType&  m_parent;       //!< The parent logic node of of this socket.
    const std::string m_name;      //!< The name of the socket.
};

//! The input socket base class for all input/output sockets of a logic node.
template<typename TConfig>
class LogicSocketInputBase : public LogicSocketBase<TConfig>
{
public:
    EXEC_GRAPH_TYPEDEF_CONFIG(TConfig);

    friend class LogicSocketOutputBase<Config>;

    template<typename... Args>
    LogicSocketInputBase(Args&&... args)
        : LogicSocketBase<TConfig>(std::forward<Args>(args)...)
    {}

    //! Cast to a logic socket of type \p SocketInputType<T>*.
    //! The cast fails at runtime if the data type \p T does not match!
    template<typename T>
    inline auto* castToType() const
    {
        EXEC_GRAPH_ASSERTMSG((this->m_type == meta::find_index<SocketTypes, T>),
                             "Types " << LogicTypes::getTypeName(this->m_type) << " and "
                                      << LogicTypes::getTypeName(meta::find_index<SocketTypes, T>) " of logic node '"
                                      << this->m_name
                                      << "' do not match!");
        return static_cast<SocketInputType<T> const*>(this);
    }

    //! Non-const overload.
    template<typename T>
    inline auto* castToType()
    {
        return const_cast<SocketInputType<T>*>(static_cast<LogicSocketInputBase const*>(this)->castToType<T>());
    }

    void setGetLink(LogicSocketOutputBase<Config>& outputSocket);

    //! Check if the socket has a Get-Link to an output socket.
    inline bool isConnectedToOutput() const { return m_from != nullptr; }

protected:
    LogicSocketOutputBase<Config>* m_from = nullptr;  //!< The single Get-Link attached to this Socket.
};

//! The input socket base class for all input/output sockets of a logic node.
template<typename TConfig>
class LogicSocketOutputBase : public LogicSocketBase<TConfig>
{
public:
    EXEC_GRAPH_TYPEDEF_CONFIG(TConfig);
    friend class LogicSocketInputBase<Config>;

    template<typename... Args>
    LogicSocketOutputBase(Args&&... args)
        : LogicSocketBase<TConfig>(std::forward<Args>(args)...)
    {}

    //! Cast to a logic socket of type \p SocketOutputType<T>*.
    //! The cast fails at runtime if the data type \p T does not match!
    template<typename T>
    inline auto* castToType() const
    {
        EXEC_GRAPH_ASSERTMSG((this->m_type == meta::find_index<SocketTypes, T>),
                             "Types " << LogicTypes::getTypeName(this->m_type) << " and "
                                      << LogicTypes::getTypeName(meta::find_index<SocketTypes, T>) " of logic node '"
                                      << this->m_name
                                      << "' do not match!");
        return static_cast<SocketOutputType<T>*>(this);
    }

    //! Non-const overload.
    template<typename T>
    inline auto* castToType()
    {
        return const_cast<SocketOutputType<T>*>(static_cast<LogicSocketOutputBase const*>(this)->castToType<T>());
    }

    void addWriteLink(LogicSocketInputBase<Config>& inputSocket);

protected:
    std::vector<LogicSocketInputBase<Config>*> m_to;  //!< All Write-Links attached to this Socket.
};

template<typename TData>
class LogicSocketData
{
public:
    using DataType = TData;

    template<typename T>
    LogicSocketData(T&& defaultValue) : m_data(std::forward<T>(defaultValue))
    {
    }

    //! Get the data value of the socket.
    inline const DataType& getValue() const { return m_data; }
    //! Non-const overload.
    inline DataType& getValue() { return m_data; }

    //! Set the data value of the socket.
    template<typename T>
    void setValue(T&& value)
    {
        m_data = std::forward<T>(value);
    }

protected:
    DataType m_data;  //!< Default value! or the output value if output socket
};

template<typename TData, typename TConfig>
class LogicSocketInput : public LogicSocketInputBase<TConfig>,
                         public LogicSocketData<TData>
{
public:
    EXEC_GRAPH_TYPEDEF_CONFIG(TConfig);
    using DataType = TData;

    /** This assert fails if the type T of the LogicSocket is
        not properly added to the type list SocketTypes in LogicSocketBase*/
    EXEC_GRAPH_STATIC_ASSERTM((!std::is_same<meta::find<SocketTypes, DataType>, meta::list<>>::value),
                              "TData is not in SocketTypes!")

    template<typename T, typename... Args>
    LogicSocketInput(T&& defaultValue, Args&&... args)
        : LogicSocketInputBase<TConfig>(meta::find_index<SocketTypes, DataType>::value, std::forward<Args>(args)...)
        , LogicSocketData<TData>(std::forward<T>(defaultValue))
    {
    }
};

template<typename TData, typename TConfig>
class LogicSocketOutput : public LogicSocketOutputBase<TConfig>,
                          public LogicSocketData<TData>
{
public:
    EXEC_GRAPH_TYPEDEF_CONFIG(TConfig);

    using DataType = TData;
    /** This assert fails if the type T of the LogicSocket is
        not properly added to the type list SocketTypes in LogicSocketBase*/
    EXEC_GRAPH_STATIC_ASSERTM((!std::is_same<meta::find<SocketTypes, DataType>, meta::list<>>::value),
                              "TData is not in SocketTypes!")

    template<typename T, typename... Args>
    LogicSocketOutput(T&& defaultValue, Args&&... args)
        : LogicSocketOutputBase<TConfig>(meta::find_index<SocketTypes, DataType>::value, std::forward<Args>(args)...)
        , LogicSocketData<TData>(std::forward<T>(defaultValue))
    {
    }
    //! Set the data value of the socket.
    template<typename T>
    void setValue(T&& value)
    {
        // Set the value
        LogicSocketData<TData>::setValue(std::forward<T>(value));
        // Forward the value to all Write-Links
        executeWriteLinks();
    }

    void executeWriteLinks();
};

} // end ExecutionGraph

// =====================================================================
// Implementation
// =====================================================================
namespace executionGraph
{
template<typename TConfig>
void LogicSocketOutputBase<TConfig>::addWriteLink(LogicSocketInputBase<TConfig>& inputSocket)
{
    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(this->getType() != inputSocket.getType(),
                                 "Output socket: " << this->getName() << " of logic node id: " << this->getParent().getId()
                                                   << " has not the same type as input socket "
                                                   << inputSocket.getName()
                                                   << " of logic node id: "
                                                   << inputSocket.getParent().getId(), NodeConnectionException);

    if (std::find(m_to.begin(), m_to.end(), &inputSocket) == m_to.end())
    {
        m_to.push_back(&inputSocket);
    }
    else
    {
        EXEC_GRAPH_THROWEXCEPTION_TYPE("Input socket: " << inputSocket.getName() << " of logic node id: "
                                                   << inputSocket.getParent().getId()
                                                   << " already added as Write-Link to logic node id: "
                                                   << this->getParent().getId(), NodeConnectionException);
    }
}

template<typename TConfig>
void LogicSocketInputBase<TConfig>::setGetLink(LogicSocketOutputBase<TConfig>& outputSocket)
{
    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(
        this->getType() != outputSocket.getType(),
        "Output socket: " << outputSocket.getName() << " of logic node id: " << outputSocket.getParent().getId()
                          << " has not the same type as input socket "
                          << this->getName()
                          << " of logic node id: "
                          << this->getParent().getId(), NodeConnectionException);

    if (!isConnectedToOutput())
    {
        m_from = &outputSocket;
    }
    else
    {
        EXEC_GRAPH_THROWEXCEPTION_TYPE("Output socket: " << outputSocket.getName() << " of logic node id: "
                                        << outputSocket.getParent().getId()
                                        << " already set as Get-Link of logic node id: "
                                        << this->getParent().getId(), NodeConnectionException);
    }
}

template<typename TData, typename TConfig>
void LogicSocketOutput<TData, TConfig>::executeWriteLinks()
{
    // Write out value to all connected (Write-Link) input sockets.
    for (auto& inputSocket : this->m_to)
    {
        // We know that this static cast is safe, since it has been
        // checked when addWriteLink() is called.
        static_cast<LogicSocketInput<TData, Config>*>(this->s)->setValue(this->m_data);  // Write data to input sockets.
    }
}

} // end executionGraph

// =====================================================================
// Macros
// =====================================================================

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
#define DECLARE_ISOCKET_TYPE(name, type) \
    typedef type IType##name;            \
    inline LogicSocket<type>* getIn_##name() { return getISocket<type>((Inputs::name)); }

#define DECLARE_OSOCKET_TYPE(name, type) \
    typedef type OType##name;            \
    inline LogicSocket<type>* getOut_##name() { return getOSocket<type>((Outputs::name)); }

#endif
