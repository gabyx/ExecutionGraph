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

#ifndef executionGraphGui_backend_BackendFactory_hpp
#define executionGraphGui_backend_BackendFactory_hpp

#include <memory>
#include <rttr/type>
#include <executionGraph/common/Factory.hpp>
#include "executionGraphGui/backend/Backend.hpp"
#include "executionGraphGui/backend/BackendRequestHandler.hpp"
#include "executionGraphGui/backend/ExecutionGraphBackend.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    Factory which makes all backends.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class BackendFactory
{
public:
    //! This factory returns the backend and a list of backend handlers.
    //! The handlers internally share the backend, and will stay consistent!
    using HandlerList = std::vector<std::shared_ptr<BackendRequestHandler>>;
    using BackendData = std::pair<std::shared_ptr<Backend>, HandlerList>;

private:
    //! The creator which creates all handlers for the ExecutionGraphBackend.
    struct CreatorExecutionGraphBackend
    {
        using Key = ExecutionGraphBackend;
        //! The actual creator function which creates all handlers and the backend for this key.
        static BackendData create(const std::path& rootPath);
    };

    //! The used factory itself.
    using Factory = executionGraph::StaticFactory<meta::list<CreatorExecutionGraphBackend>>;

public:
    //! Create all handlers for the backend `BackendType`.
    template<typename BackendType, typename... Args>
    static BackendData Create(Args&&... args) { return Factory::create<BackendType>(std::forward<Args>(args)...); }

    //! Create all handlers for the backend type given by an `rttr::type`.
    template<typename... Args>
    static std::optional<BackendData> Create(const rttr::type& type, Args&&... args)
    {
        return Factory::create(type, std::forward<Args>(args)...);
    }
};

#endif