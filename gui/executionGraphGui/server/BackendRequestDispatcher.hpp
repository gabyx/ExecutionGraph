// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Dec 19 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGui_server_BackendRequestDispatcher_hpp
#define executionGraphGui_server_BackendRequestDispatcher_hpp

#include "executionGraphGui/backend/BackendRequestHandler.hpp"
#include "executionGraphGui/common/RequestDispatcher.hpp"
#include "executionGraphGui/server/BackendRequest.hpp"
#include "executionGraphGui/server/BackendResponse.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    The actual backend request dispatcher.
    Request and Response should be cheap movable.

    @date Sun Mar 11 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class BackendRequestDispatcher final : public RequestDispatcher<BackendRequestHandler,
                                                                BackendRequest,
                                                                BackendResponsePromise,
                                                                false>
{
public:
    using Base = RequestDispatcher<BackendRequestHandler,
                                   BackendRequest,
                                   BackendResponsePromise,
                                   false>;

public:
    template<typename... Args>
    BackendRequestDispatcher(Args&&... args)
        : Base(std::forward<Args>(args)...)
    {}

    virtual ~BackendRequestDispatcher() = default;
};

#endif