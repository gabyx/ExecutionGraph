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

#include "backend/DummyBackendRequestHandler.hpp"
#include <chrono>
#include <common/Loggers.hpp>

namespace
{
    const std::string c_debugResponse = "Hello from ExecutionGraphBackend!";
}

bool DummyBackendRequestHandler::handleRequest(const Request& m_request, ResponsePromise& m_response)
{
    EXECGRAPHGUI_BACKENDLOG_INFO("DummyBackendRequestHandler::handleRequest");
    using namespace std::chrono_literals;

    EXECGRAPHGUI_BACKENDLOG_INFO("DummyBackendRequestHandler: Computing started [1sec] ...");
    std::this_thread::sleep_for(1.0s);
    EXECGRAPHGUI_BACKENDLOG_INFO("DummyBackendRequestHandler: Computing finished!");

    EXECGRAPH_THROW_EXCEPTION_IF(std::rand() % 2 == 0, "DummyBackendRequestHandler: Spontanously deciced to throw a meaningless exception! Because its fun!")

    BinaryBuffer<BufferPool> buffer(c_debugResponse.size(), m_response.getAllocator());

    auto it = buffer.begin();
    for(const char& v : c_debugResponse)
    {
        *it = v;
        ++it;
    }
    EXECGRAPHGUI_BACKENDLOG_INFO("DummyBackendRequestHandler: Allocated Payload");

    // Set the response ready!
    m_response.setReady(ResponsePromise::Payload{std::move(buffer), "application/octet-stream"});

    return true;
}
