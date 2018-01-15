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

#include "backend/BackendMessageHandler.hpp"
#include <executionGraph/common/ObjectID.hpp>

class Backend : public executionGraph::ObjectID
{
public:    
    using Id = executionGraph::ObjectID::Id;
    using Handler = BackendMessageHandler;
    using HandlerList = std::vector<Handler*>;
public:
    Backend(const Id& id) : executionGraph::ObjectID(id) {}
    virtual ~Backend() override = default;

public:
    //! Get the Messagehandlers
    virtual HandlerList getMessageHandlers() = 0; 
};

#endif
