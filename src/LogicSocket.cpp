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

#include "executionGraph/nodes/LogicSocket.hpp"

namespace executionGraph
{
    LogicSocketInputBase::~LogicSocketInputBase() noexcept
    {
        EXECGRAPH_LOG_TRACE("Destructor: LogicSocketInputBase: index: '{0}', parent: '{1}'",
                            this->getIndex(),
                            fmt::ptr(&this->parent()));

    }

    LogicSocketOutputBase::~LogicSocketOutputBase() noexcept
    {
        EXECGRAPH_LOG_TRACE("Destructor: LogicSocketOutputBase: index: '{0}', parent: '{1}'",
                            this->getIndex(),
                            fmt::ptr(&this->parent()));

      
    }
}  // namespace executionGraph