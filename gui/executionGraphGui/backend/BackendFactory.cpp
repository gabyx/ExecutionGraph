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

#include "backend/BackendFactory.hpp"
#include <utility>
#include "backend/DummyBackendRequestHandler.hpp"

BackendFactory::BackendData
BackendFactory::CreatorExecutionGraphBackend::create()
{
    // create the executionGraph backend
    auto backend = std::make_shared<ExecutionGraphBackend>();

    // create a simple dummy handler
    auto dummyHandler = std::make_shared<DummyBackendRequestHandler>(backend);
    return BackendData{backend, {dummyHandler}};
}