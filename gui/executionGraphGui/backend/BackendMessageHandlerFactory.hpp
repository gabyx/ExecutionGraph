//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 15 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraphGui_backend_BackendMessageHandlerFactory_hpp
#define executionGraphGui_backend_BackendMessageHandlerFactory_hpp

#include <executionGraph/common/Factory.hpp>
#include <rttr/type>
#include "backend/BackendMessageHandler.hpp"
#include "backend/ExecutionGraphBackend.hpp"

class BackendMessageHandlerFactory
{
public:
    //! This factory returns a list of backend handlers.
    using HandlerList = std::vector<std::shared_ptr<BackendMessageHandler>>;

private:
    //! The creator which creates all handlers for the ExecutionGraphBackend.
    struct CreatorExecutionGraphBackend
    {
        using Key = ExecutionGraphBackend;
        //! The actual creator function which creats all handlers for this key.
        static HandlerList create();
    };

    //! The used factory itself.
    using Factory = executionGraph::StaticFactory<meta::list<CreatorExecutionGraphBackend>>;

public:
    //! Create all handlers for the backend `BackendType`.
    template<typename BackendType>
    static HandlerList Create() { return Factory::create<BackendType>(); }
    //! Create all handlers for the backend type given by an `rttr::type`.
    static HandlerList Create(const rttr::type& type) { return Factory::create(type); }
};

#endif