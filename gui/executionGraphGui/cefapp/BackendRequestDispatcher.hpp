// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Mar 11 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGui_cefapp_BackendRequestDispatcher_hpp
#define executionGraphGui_cefapp_BackendRequestDispatcher_hpp

#include <memory>
#include "executionGraphGui/backend/BackendRequestHandler.hpp"
#include "executionGraphGui/cefapp/RequestCef.hpp"
#include "executionGraphGui/cefapp/ResponsePromiseCef.hpp"
#include "executionGraphGui/common/RequestDispatcher.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    The actual backend request dispatcher.

    @date Sun Mar 11 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class BackendRequestDispatcher final : public RequestDispatcher<BackendRequestHandler,
                                                                RequestCef,
                                                                ResponsePromiseCef>
{
public:
    using Base = RequestDispatcher<BackendRequestHandler,
                                   RequestCef,
                                   ResponsePromiseCef>;

public:
    template<typename... Args>
    BackendRequestDispatcher(Args&&... args)
        : Base(std::forward<Args>(args)...)
    {}

    virtual ~BackendRequestDispatcher() = default;
};
#endif