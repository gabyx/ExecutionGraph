// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jul 29 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGUI_common_Exception_hpp
#define executionGraphGUI_common_Exception_hpp

#include <exception>
#include <executionGraph/common/Exception.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif

#define EXECGRAPHGUI_THROW_EXCEPTION(...) EXECGRAPH_THROW_EXCEPTION(__VA_ARGS__)
#define EXECGRAPHGUI_THROW_EXCEPTION_IF(condition, ...) EXECGRAPH_THROW_EXCEPTION_TYPE_IF(condition, executionGraph::Exception, __VA_ARGS__)
#define EXECGRAPHGUI_THROW_EXCEPTION_TYPE(Type, ...) EXECGRAPH_THROW_EXCEPTION_TYPE(Type, __VA_ARGS__)
#define EXECGRAPHGUI_THROW_EXCEPTION_TYPE_IF(condition, Type, ...) EXECGRAPH_THROW_EXCEPTION_TYPE_IF(condition, Type, __VA_ARGS__)

#ifdef __clang__
#    pragma clang diagnostic pop
#endif

class InternalBackendError final : public executionGraph::Exception
{
public:
    InternalBackendError(const std::string& s)
        : executionGraph::Exception(s) {}
};

#endif
