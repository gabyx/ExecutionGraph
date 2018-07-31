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

#ifndef executionGraphGUI_backend_requestHandlers_RequestError_hpp
#define executionGraphGUI_backend_requestHandlers_RequestError_hpp

#include "executionGraphGUI/common/Exception.hpp"

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
    BadRequestError(const std::stringstream& ss)
        : executionGraph::Exception(ss) {}
};

#endif