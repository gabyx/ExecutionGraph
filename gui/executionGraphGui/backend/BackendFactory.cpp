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

#include "executionGraphGui/backend/BackendFactory.hpp"
#include <utility>
#include "executionGraphGui/backend/requestHandlers/DummyRequestHandler.hpp"
#include "executionGraphGui/backend/requestHandlers/GeneralInfoRequestHandler.hpp"
#include "executionGraphGui/backend/requestHandlers/GraphManagementRequestHandler.hpp"
#include "executionGraphGui/backend/requestHandlers/GraphManipulationRequestHandler.hpp"

BackendFactory::BackendData
BackendFactory::CreatorExecutionGraphBackend::create()
{
    // Create the executionGraph backend
    auto backend = std::make_shared<ExecutionGraphBackend>();

    // Create a simple dummy handler
    auto dummyHandler = std::make_shared<DummyRequestHandler>(backend);

    // Create a general info handler
    auto generalInfoHandler = std::make_shared<GeneralInfoRequestHandler>(backend);

    // Create a graph manipulation handler
    auto graphManipHandler = std::make_shared<GraphManipulationRequestHandler>(backend);

    // Create a graph management handler
    auto graphManagementHandler = std::make_shared<GraphManagementRequestHandler>(backend);

    return BackendData{backend,
                       {dummyHandler,
                        generalInfoHandler,
                        graphManipHandler,
                        graphManagementHandler}};
}