//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Sun Jan 14 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#pragma once

#include <rttr/type>
#include "executionGraphGui/common/IObjectID.hpp"

class Backend : public executionGraph::IObjectID
{
    RTTR_ENABLE()
    EXECGRAPH_NAMED_OBJECT_ID_DECLARATION

protected:
    Backend(const IdNamed& id)
        : m_id(id) {}
    virtual ~Backend() override = default;
};
