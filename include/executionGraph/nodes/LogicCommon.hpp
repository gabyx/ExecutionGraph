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
#include "executionGraph/nodes/LogicSocketDefaultTypes.hpp"

#define EXECGRAPH_DEFINE_TYPES()                   \
    using NodeId      = executionGraph::NodeId;    \
    using IndexType   = executionGraph::IndexType; \
    using SocketIndex = executionGraph::SocketIndex;

namespace executionGraph
{
    template<typename T>
    using SocketPointer = std::unique_ptr<T, void (*)(T*)>;  //! The general socket pointer type.

    using IndexType   = uint64_t;   //! A general index type.
    using NodeId      = uint64_t;   //! Node Id type.
    using SocketIndex = IndexType;  //! The socket index type.

}  // namespace executionGraph
