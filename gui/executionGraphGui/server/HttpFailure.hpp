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

// Report a failure
template<typename T>
void fail(boost::system::error_code ec, const T& what)
{
    EXECGRAPHGUI_BACKENDLOG_ERROR("Failure: {0} : {1}", what, ec.message());
}

#endif