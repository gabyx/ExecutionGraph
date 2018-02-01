//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Thu Feb 01 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#include <cef_base.h>
#include <executionGraph/common/Identifier.hpp>
#include <memory>
#include <unordered_map>
#include <vector>
class CefMessageRouterBrowserSide;
class Backend;
class BackendMessageHandler;

//! Registering for Backends
class BackendStorage final
{
public:
    using Id = executionGraph::Id;

public:
    BackendStorage()  = default;
    ~BackendStorage() = default;

public:
    void RegisterBackend(const std::shared_ptr<Backend>& backend);
    std::shared_ptr<Backend> GetBackend(const Id& id) const;
    const std::unordered_map<Id, std::shared_ptr<Backend>>& GetBackends() { return m_backends; }

    void RegisterHandlersAtRouter(CefRefPtr<CefMessageRouterBrowserSide> router);
    void UnregisterHandlersFromRouter(CefRefPtr<CefMessageRouterBrowserSide> router);

private:
    std::unordered_map<Id, std::shared_ptr<Backend>> m_backends;                                    //!< All backends.
    std::unordered_map<Id, std::vector<std::shared_ptr<BackendMessageHandler>>> m_backendHandlers;  //!< All handlers for every backend.
};