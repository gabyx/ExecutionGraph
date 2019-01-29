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
#include "executionGraphGui/backend/requestHandlers/FileBrowserRequestHandler.hpp"
#include "executionGraphGui/backend/requestHandlers/GeneralInfoRequestHandler.hpp"
#include "executionGraphGui/backend/requestHandlers/GraphManagementRequestHandler.hpp"
#include "executionGraphGui/backend/requestHandlers/GraphManipulationRequestHandler.hpp"

BackendFactory::BackendData
BackendFactory::CreatorExecutionGraphBackend::create(const std::path& rootPath)
{
    // Create the executionGraph backend
    auto backend = std::make_shared<ExecutionGraphBackend>(rootPath);

    // Create a general info handler
    auto generalInfoHandler = std::make_shared<GeneralInfoRequestHandler>(backend);

    // Create a graph manipulation handler
    auto graphManipHandler = std::make_shared<GraphManipulationRequestHandler>(backend);

    // Create a graph management handler
    auto graphManagementHandler = std::make_shared<GraphManagementRequestHandler>(backend);

    // Create file-browser handler
    auto fileBrowserHandler = std::make_shared<FileBrowserRequestHandler>(backend, rootPath);

    return BackendData{backend,
                       {generalInfoHandler,
                        graphManipHandler,
                        graphManagementHandler,
                        fileBrowserHandler}};
}