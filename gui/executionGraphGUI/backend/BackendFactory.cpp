//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Tue Jan 30 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#include "executionGraphGUI/backend/BackendFactory.hpp"
#include <utility>
#include "executionGraphGUI/backend/requestHandlers/DummyRequestHandler.hpp"
#include "executionGraphGUI/backend/requestHandlers/GeneralInfoRequestHandler.hpp"

BackendFactory::BackendData
BackendFactory::CreatorExecutionGraphBackend::create()
{
    // Create the executionGraph backend
    auto backend = std::make_shared<ExecutionGraphBackend>();

    // Create a simple dummy handler
    auto dummyHandler = std::make_shared<DummyRequestHandler>(backend);

    // Create a graph handler (add/remove graphs, graph info etc.)
    auto graphHandler = std::make_shared<GeneralInfoRequestHandler>(backend);

    return BackendData{backend, {dummyHandler, graphHandler}};
}