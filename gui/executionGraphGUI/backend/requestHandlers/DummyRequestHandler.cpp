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

#include "executionGraphGUI/backend/requestHandlers/DummyRequestHandler.hpp"
#include <chrono>
#include "executionGraphGUI/common/BinaryBuffer.hpp"
#include "executionGraphGUI/common/Loggers.hpp"
#include "executionGraphGUI/common/RequestError.hpp"

namespace
{
    const std::string c_debugResponse                    = "Hello from ExecutionGraphBackend!";
    const std::unordered_set<std::string> c_requestTypes = {"general/dummyRequest"};
}  // namespace

const std::unordered_set<std::string>& DummyRequestHandler::getRequestTypes() const
{
    return c_requestTypes;
}

void DummyRequestHandler::handleRequest(const Request& request, ResponsePromise& response)
{
    EXECGRAPHGUI_BACKENDLOG_INFO("DummyRequestHandler::handleRequest");
    using namespace std::chrono_literals;

    EXECGRAPHGUI_BACKENDLOG_INFO("DummyRequestHandler: Computing started [1sec] ...");
    std::this_thread::sleep_for(1.0s);
    EXECGRAPHGUI_BACKENDLOG_INFO("DummyRequestHandler: Computing finished!");

    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(std::rand() % 2 == 0,
                                      "DummyRequestHandler: Spontanously deciced to throw a meaningless exception! Because its fun!")

    auto buffer = makeBinaryBuffer(response.getAllocator(), c_debugResponse.size());

    auto it = buffer.begin();
    for(const char& v : c_debugResponse)
    {
        *it = v;
        ++it;
    }
    EXECGRAPHGUI_BACKENDLOG_INFO("DummyRequestHandler: Allocated Payload");

    // Set the response ready!
    response.setReady(ResponsePromise::Payload{std::move(buffer), "application/octet-stream"});
}
