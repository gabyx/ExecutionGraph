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
#include <unordered_set>

#include "ExecutionGraph/common/Asserts.hpp"
#include "ExecutionGraph/common/DemangleTypes.hpp"
#include "ExecutionGraph/common/EnumClassHelper.hpp"
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

    LogicSocketBase(IndexType type,
                    SocketIndex index,
                    NodeBaseType& parent,
                    const std::string& name = "")
        : m_type(type)
        , m_index(index)
        , m_parent(parent)
        , m_name((name.empty()) ? name : "[" + std::to_string(index) + "]")
        , m_defaultValueId(type)
    {
    }

    IndexType getType() const { return m_type; }
    SocketIndex getIndex() const { return m_index; }
    std::string getName() const { return m_name; }

    const NodeBaseType& getParent() const { return m_parent; }
    NodeBaseType& getParent() { return m_parent; }

protected:
    const IndexType m_type;     //!< The index in to the meta::list SocketTypes, which type this is!
    const SocketIndex m_index;  //!< The index of the slot at which this socket is installed in a LogicNode.
    NodeBaseType& m_parent;     //!< The parent logic node of of this socket.
    const std::string m_name;   //!< The name of the socket.

    std::string getNameOfType() const;  //!< Returns the name of the current type.
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
    {
    }

    //! Cast to a logic socket of type \p SocketInputType<T>*.
    //! The cast fails at runtime if the data type \p T does not match!
    template<typename T>
    auto* castToType() const
    {
        EXEC_GRAPH_THROW_BADSOCKETCAST_IF((this->m_type != meta::find_index<SocketTypes, T>::value),
                                          "Casting socket index '" << this->m_index << "' with type '" << this->getNameOfType() << "' into '"
                                                                   << demangle<T>()
                                                                   << "' of logic node id: '"
                                                                   << this->m_parent.getId()
                                                                   << "' which is wrong!");

        return static_cast<SocketInputType<T> const*>(this);
    }

    //! Non-const overload.
    template<typename T>
    auto* castToType()
    {
        return const_cast<SocketInputType<T>*>(static_cast<LogicSocketInputBase const*>(this)->castToType<T>());
    }

    //! Set the Get-Link to an output socket.
    void setGetLink(LogicSocketOutputBase<Config>& outputSocket);
    //! Check if the socket has a Get-Link to an output socket.
    bool hasGetLink() const { return m_getFrom != nullptr; }
    //! Get the output socket to which the Get-Link points.
    LogicSocketOutputBase<Config>* followGetLink() { return m_getFrom; }

    //! Get all sockets writing to this input socket.
    const auto& getWritingSockets() const { return m_writingParents; }
    //! Get the connection count of this input socket.
    IndexType getConnectionCount() const { return (hasGetLink() ? 1 : 0) + m_writingParents.size(); }

protected:
    //! The default output socket id. This defaults to `m_type` since for every type
    //! in SocketTypes there needs to be a default socket which is used as default.
    const IndexType m_defaultOutputSocketId; 

    LogicSocketOutputBase<Config>* m_getFrom = nullptr;                   //!< The single Get-Link attached to this Socket.
    void const* m_data = nullptr;                                         //!< The pointer to the actual data of this input node.
    std::unordered_set<LogicSocketOutputBase<Config>*> m_writingParents;  //!< All parent output sockets which write to this input.
};

//! The input socket base class for all input/output sockets of a logic node.
template<typename TConfig>
class LogicSocketOutputBase : public LogicSocketBase<TConfig>
{
public:
    EXEC_GRAPH_TYPEDEF_CONFIG(TConfig);
    friend class LogicSocketInputBase<Config>;

    template<typename T, typename... Args>
    LogicSocketOutputBase(const T& data, Args&&... args)
        : LogicSocketBase<TConfig>(std::forward<Args>(args)...), m_data(static_cast<const void*>(&data))
    {  
    }

    ~LogicSocketOutputBase(){
        // Reset data address in all input sockets.
        for(auto* inSocket : m_getterChilds)
        {
            inSocket->m_data = nullptr;
        }
    }

    //! Cast to a logic socket of type \p SocketOutputType<T>*.
    //! The cast fails at runtime (if NDEBUG defined) if the data type \p T does not match!
    template<typename T>
    auto* castToType() const
    {
        EXEC_GRAPH_THROW_BADSOCKETCAST_IF((this->m_type != meta::find_index<SocketTypes, T>::value),
                                          "Casting socket index '" << this->m_index << "' with type '" << this->getNameOfType() << "' into '"
                                                                   << demangle<T>()
                                                                   << "' of logic node id: '"
                                                                   << this->m_parent.getId()
                                                                   << "' which is wrong!");

        return static_cast<SocketOutputType<T> const*>(this);
    }

    //! Non-const overload.
    template<typename T>
    auto* castToType()
    {
        return const_cast<SocketOutputType<T>*>(static_cast<LogicSocketOutputBase const*>(this)->castToType<T>());
    }

    void addWriteLink(LogicSocketInputBase<Config>& inputSocket);

    const auto& getGetterSockets() { return m_getterChilds; }
    IndexType getConnectionCount() { return m_writeTo.size() + m_getterChilds.size(); }

protected:
    void executeWriteLinks()
    {
        // Write out value to all connected (Write-Link) input sockets.
        for (auto* inputSocket : this->m_writeTo)
        {
            inputSocket->m_data = m_data;  // Set data pointer in input socket.
        }
    }

protected:
    std::vector<LogicSocketInputBase<Config>*> m_writeTo;              //!< All Write-Links attached to this Socket.
    std::unordered_set<LogicSocketInputBase<Config>*> m_getterChilds;  //!< All child sockets which have a Get-Link to this socket.
    void const * const m_data = nullptr;                               //!< The raw pointer to the actual data of this output socket.
};


template<typename TData, typename TConfig>
class LogicSocketInput final  public LogicSocketInputBase<TConfig>
{
public:
    EXEC_GRAPH_TYPEDEF_CONFIG(TConfig);
    using DataType = TData;

    /** This assert fails if the type T of the LogicSocket is
        not properly added to the type list SocketTypes in LogicSocketBase*/
    static_assert(!std::is_same<meta::find<SocketTypes, DataType>, meta::list<>>::value,
                  "TData is not in SocketTypes!");

    template<typename... Args>
    LogicSocketInput(Args&&... args)
        LogicSocketInputBase<TConfig>(meta::find_index<SocketTypes, DataType>::value, std::forward<Args>(args)...)
    {
    }

    //! Get the data value of the socket. (follow Get-Link). 
    //! If this input socket has not been connected, this results in an access violation!
    //! The graph checks that all input nodes ar connected when solving the execution order!
    const DataType& getValue() const
    {
        EXEC_GRAPH_ASSERT(m_data, "Input socket: " << this->getName() << 
                                  " of logic node id: " << this->getParent()->getId() " not connected");
        return *static_cast<DataType*>(m_data);
    }
    //! Non-const overload.
    DataType& getValue()
    {
        return const_cast<DataType&>(static_cast<const LogicSocketInput*>(this)->getValue());
    }
};

template<typename TData, typename TConfig>
class LogicSocketOutput final : public LogicSocketData<TData>,
                                public LogicSocketOutputBase<TConfig>
{
public:
    EXEC_GRAPH_TYPEDEF_CONFIG(TConfig);

    using DataType = TData;
    /** This assert fails if the type T of the LogicSocket is
        not properly added to the type list SocketTypes in LogicSocketBase*/
    static_assert(!std::is_same<meta::find<SocketTypes, DataType>, meta::list<>>::value,
                  "TData is not in SocketTypes!");

    template<typename T, typename... Args>
    LogicSocketOutput(T&& initValue, Args&&... args)
        : m_data(std::forward<T>(initValue))
        , LogicSocketOutputBase<TConfig>(getValue(), meta::find_index<SocketTypes, DataType>::value, std::forward<Args>(args)...)
    {
    }

    //! Set the data value of the socket.
    template<typename T>
    void setValue(T&& value)
    {
        // Set the value
        m_data = std::forward<T>(value);
        // Forward the value to all Write-Links
        executeWriteLinks();
    }

    //! Get the data value of the socket.
    const DataType& getValue() const { return m_data; }
    //! Non-const overload.
    DataType& getValue() { return m_data; }

private:
    DataType m_data;  //!< The output value
};

}  // end ExecutionGraph

// =====================================================================
// Implementation
// =====================================================================
namespace executionGraph
{

template<typename TConfig>
std::string LogicSocketBase<TConfig>::getNameOfType() const
{
    std::string s = "'type-not-found'";
    IndexType i   = 0;
    auto f        = [&](auto type) {
        if (i == m_type)
        {
            s = demangle(type);
        }
        i++;
    };

    meta::for_each(SocketTypes{}, f);
    return s;
}

template<typename TConfig>
void LogicSocketOutputBase<TConfig>::addWriteLink(LogicSocketInputBase<TConfig>& inputSocket)
{
    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(inputSocket.getParent().getId() == this->getParent().getId(),
                                      "No Write-Link connection to our input slot! (node id: " << this->getParent().getId() << ")",
                                      NodeConnectionException);

    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(this->getType() != inputSocket.getType(),
                                      "Output socket: " << this->getName() << " of logic node id: " << this->getParent().getId()
                                                        << " has not the same type as input socket "
                                                        << inputSocket.getName()
                                                        << " of logic node id: "
                                                        << inputSocket.getParent().getId(),
                                      NodeConnectionException);

    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(m_getterChilds.find(&inputSocket) != m_getterChilds.end(),
                                      "Cannot add Write-Link from output socket: "
                                          << this->getName()
                                          << " of logic node id: "
                                          << this->getParent().getId()
                                          << " to "
                                          << inputSocket.getName()
                                          << " of logic node id: "
                                          << inputSocket.getParent().getId()
                                          << "because input already has a Get-Link to this output.",
                                      NodeConnectionException);

    if (std::find(m_writeTo.begin(), m_writeTo.end(), &inputSocket) == m_writeTo.end())
    {
        m_writeTo.push_back(&inputSocket);
        inputSocket.m_writingParents.emplace(this);
    }
    else
    {
        EXEC_GRAPH_THROWEXCEPTION_TYPE("Input socket: " << inputSocket.getName() << " of logic node id: "
                                                        << inputSocket.getParent().getId()
                                                        << " already added as Write-Link to logic node id: "
                                                        << this->getParent().getId(),
                                       NodeConnectionException);
    }
}

template<typename TConfig>
void LogicSocketInputBase<TConfig>::setGetLink(LogicSocketOutputBase<TConfig>& outputSocket)
{
    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(outputSocket.getParent().getId() == this->getParent().getId(),
                                      "No Get-Link connection to our output slot! (node id: " << this->getParent().getId() << ")",
                                      NodeConnectionException);

    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(this->getType() != outputSocket.getType(),
                                      "Output socket: " << outputSocket.getName() << " of logic node id: " << outputSocket.getParent().getId()
                                                        << " has not the same type as input socket "
                                                        << this->getName()
                                                        << " of logic node id: "
                                                        << this->getParent().getId(),
                                      NodeConnectionException);

    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(m_writingParents.find(&outputSocket) != m_writingParents.end(),
                                      "Cannot add Get-Link from input socket: "
                                          << this->getName()
                                          << " of logic node id: "
                                          << this->getParent().getId()
                                          << " to "
                                          << outputSocket.getName()
                                          << " of logic node id: "
                                          << outputSocket.getParent().getId()
                                          << "because output already has a Write-Link to this input.",
                                      NodeConnectionException);

    EXEC_GRAPH_THROWEXCEPTION_TYPE_IF(hasGetLink(), 
    "Get-Link of logic node id: " << this->getParent().getId()
                                                                     << " already set!",
                                       NodeConnectionException);

    //std::cout << "Add Get-Link: " << outputSocket.getParent().getId() << outputSocket.getName() << " --> " << this->getParent().getId() << this->getName() << std::endl;
    m_getFrom = &outputSocket;
    m_data = &outputSocket.getValue(); // Set data pointer of this input socket. 
    outputSocket.m_getterChilds.emplace(this);
}

}  // end executionGraph
#endif
