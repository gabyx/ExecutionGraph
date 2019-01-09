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

#ifndef executionGraphGui_backend_requestHandlers_FileBrowserRequestHandler_hpp
#define executionGraphGui_backend_requestHandlers_FileBrowserRequestHandler_hpp

#include "executionGraphGui/backend/BackendRequestHandler.hpp"
#include "executionGraphGui/common/FunctionMap.hpp"

class ExecutionGraphBackend;

/* ---------------------------------------------------------------------------------------*/
/*!
    Request handler for information on graphs in the backend.

    Handles the request URLs: 
        - "/eg-backend/files/browse"

    @date Sat Jul 07 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/

class FileBrowserRequestHandler final : public BackendRequestHandler
{
    RTTR_ENABLE(BackendRequestHandler)

public:
    using IdNamed  = BackendRequestHandler::IdNamed;
    using Function = std::function<void(FileBrowserRequestHandler&,
                                        const Request& request,
                                        ResponsePromise& response)>;

public:
    FileBrowserRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                              std::path rootPath,
                              const IdNamed& id = IdNamed("FileBrowserRequestHandler"));

    void handleRequest(const Request& request, ResponsePromise& response) override;
    const std::unordered_set<std::string>& getRequestTypes() const override;

private:
    void handle(const Request& request, ResponsePromise& response);

    void handleBrowse(const Request& request, ResponsePromise& response);

private:
    static FunctionMap<Function> initFunctionMap();
    static const FunctionMap<Function> m_functionMap;

    std::shared_ptr<ExecutionGraphBackend> m_backend;
    std::path m_rootPath;
};

#endif
