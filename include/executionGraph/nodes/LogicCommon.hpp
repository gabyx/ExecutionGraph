//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 08 2018
//!  @author Gabriel Nützi, <gnuetzi (at) gmail (døt) com>
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#pragma once

#include <array>
#include <memory>
#include <string>
#include <meta/meta.hpp>
#include "executionGraph/common/EnumClassHelper.hpp"
#include "executionGraph/common/SfinaeMacros.hpp"
#include "executionGraph/common/TypeDefs.hpp"

#define EG_DEFINE_TYPES()                              \
    using NodeId       = executionGraph::NodeId;       \
    using SocketDataId = executionGraph::SocketDataId; \
    using IndexType    = executionGraph::IndexType;    \
    using SocketIndex  = executionGraph::SocketIndex

namespace executionGraph
{
    /*! Forward declarations of fixed types. */
    //@{
    class LogicNode;

    template<typename IsInput>
    class LogicSocketBase;
    using LogicSocketInputBase  = LogicSocketBase<meta::bool_<true>>;
    using LogicSocketOutputBase = LogicSocketBase<meta::bool_<false>>;

    template<typename T>
    class LogicSocketInput;
    template<typename T>
    class LogicSocketOutput;

    template<typename Config, typename Derived>
    class LogicSocketConnections;

    using IndexType    = uint64_t;   //! A general index type.
    using NodeId       = IndexType;  //! Node Id type.
    using SocketDataId = IndexType;  //! DataNode Id type.

    static constexpr NodeId nodeIdInvalid             = std::numeric_limits<IndexType>::max();
    static constexpr SocketDataId socketDataIdInvalid = std::numeric_limits<IndexType>::max();

    using SocketIndex = IndexType;  //! The socket index type.

    class LogicSocketDataBase;

    template<typename Data>
    class LogicSocketData;

    template<typename Data>
    class ILogicSocketDataAccess;

    template<typename ConnectionTraits>
    class LogicSocketDataConnectionsBase;

    template<typename ConnectionTraits, typename Derived>
    class LogicSocketDataConnections;
    //@}

    /*! Forward declarations of user-defined types (@todo make available). 
        See https://wandbox.org/permlink/KiZnp9QHafZuvoXa
    */
    //@{
    template<typename TData>
    struct ConnectionTraits
    {
        using Data                    = TData;
        using InputSocket             = LogicSocketInput<Data>;
        using OutputSocket            = LogicSocketOutput<Data>;
        using InputSocketConnections  = LogicSocketConnections<ConnectionTraits, InputSocket>;
        using OutputSocketConnections = LogicSocketConnections<ConnectionTraits, OutputSocket>;

        using SocketData            = LogicSocketData<Data>;
        using SocketDataConnections = LogicSocketDataConnections<ConnectionTraits, SocketData>;

        using SocketDataBase            = typename LogicSocketData<Data>::Base;
        using ISocketDataAccess         = ILogicSocketDataAccess<Data>;
        using SocketDataConnectionsBase = LogicSocketDataConnectionsBase<ConnectionTraits>;
    };
    //@}

}  // namespace executionGraph
