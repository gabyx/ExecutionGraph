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

#ifndef executionGraphGui_backend_Backend_hpp
#define executionGraphGui_backend_Backend_hpp

#include <executionGraph/common/IObjectID.hpp>
#include <rttr/type>

class Backend : public executionGraph::IObjectID
{
    RTTR_ENABLE()
    EXECGRAPH_OBJECT_ID_DECLARATION

protected:
    Backend(const Id& id)
        : m_id(id) {}
    virtual ~Backend() override = default;
};

#endif
