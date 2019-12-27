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
#include "executionGraph/common/EnumClassHelper.hpp"
#include "executionGraph/common/SfinaeMacros.hpp"
#include "executionGraph/common/TypeDefs.hpp"

#define EXECGRAPH_DEFINE_TYPES()                    \
    using NodeId      = executionGraph::NodeId;     \
    using NodeDataId  = executionGraph::NodeDataId; \
    using IndexType   = executionGraph::IndexType;  \
    using SocketIndex = executionGraph::SocketIndex

namespace executionGraph
{
    // Forward declarations
    class LogicNode;
    
    class LogicSocketBase;
    class LogicSocketInputBase;
    class LogicSocketOutputBase;
    template<typename T>
    class LogicSocketInput;
    template<typename T>
    class LogicSocketOutput;

    class LogicNodeDataBase;
    template<typename T>
    class LogicNodeData;

    using IndexType  = uint64_t;   //! A general index type.
    using NodeId     = IndexType;  //! Node Id type.
    using NodeDataId = IndexType;  //! DataNode Id type.

    static constexpr NodeId nodeIdInvalid         = std::numeric_limits<IndexType>::max();
    static constexpr NodeDataId nodeDataIdInvalid = std::numeric_limits<IndexType>::max();

    using SocketIndex = IndexType;  //! The socket index type.

}  // namespace executionGraph
