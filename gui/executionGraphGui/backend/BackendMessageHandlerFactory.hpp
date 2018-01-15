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

#include <rttr/type.h>

class BackendHandlersCreator

    class BackendMessageHandlerFactory
{
    using HandlerList     = std::vector<std::shared_ptr<BackendMessageHandler>>;
    using CreatorFunction = HandlerList (*)();

    template<typename Creator>
    RegisterCreator(const rttr::type& type)
    {
        // If this does not compile your type `Creator` does not have a static
        // create function of type `CreatorFunction`.
        m_creatorMap.emplace(type, static_cast<CreatorFunction>(&Creator::create));
    }

    static CreateMessageHandlersFor(const rttr::type& type)
    {
        create
    }

private:
    static executionGraph::Factory<rttr::type, HandlerList> m_factory
};

#endif