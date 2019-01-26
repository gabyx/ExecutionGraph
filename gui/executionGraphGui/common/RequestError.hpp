// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Tue Jul 31 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include "executionGraphGui/common/Exception.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    Error exception for a bad request.

    @date Tue Jul 31 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class BadRequestError final : public executionGraph::Exception
{
public:
    BadRequestError(const std::string& s)
        : executionGraph::Exception(s) {}
};

#define EXECGRAPHGUI_THROW_BAD_REQUEST_IF(condition, ...) EXECGRAPHGUI_THROW_TYPE_IF(condition, BadRequestError, __VA_ARGS__)
#define EXECGRAPHGUI_THROW_BAD_REQUEST(...) EXECGRAPHGUI_THROW_TYPE(BadRequestError, __VA_ARGS__)
