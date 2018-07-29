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

#ifndef executionGraphGui_backend_requestHandlers_DummyRequestHandler_hpp
#define executionGraphGui_backend_requestHandlers_DummyRequestHandler_hpp

#include <backend/BackendRequestHandler.hpp>
#include <backend/ExecutionGraphBackend.hpp>

class DummyRequestHandler final : public BackendRequestHandler
{
    RTTR_ENABLE(BackendRequestHandler)

public:
    using Id = BackendRequestHandler::Id;

public:
    DummyRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                        const Id& id = "DummyRequestHandler")
        : BackendRequestHandler(id)
    {
    }

public:
    void handleRequest(const Request& request, ResponsePromise& response) override;

public:
    const std::unordered_set<std::string>& getRequestTypes() const override;

private:
    std::shared_ptr<ExecutionGraphBackend> m_backend;
};

#endif
