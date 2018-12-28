// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Thu Dec 13 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGui_server_HttpFailure_hpp
#define executionGraphGui_server_HttpFailure_hpp

#include <boost/system/error_code.hpp>
#include "executionGraphGui/common/Loggers.hpp"
#include "executionGraphGui/common/Exception.hpp"

//! Report a failure.
template<bool doThrow = false, typename ErrorCode, typename T>
void fail(const ErrorCode& ec, const T& what)
{
    if constexpr(doThrow)
    {
        EXECGRAPH_THROW("Failure: {0} : {1}", what, ec.message());
    }
    else
    {
        EXECGRAPHGUI_BACKENDLOG_ERROR("Failure: {0} : {1}", what, ec.message());
    }
}

#endif