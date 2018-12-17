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

#ifndef executionGraphGui_common_Exception_hpp
#define executionGraphGui_common_Exception_hpp

#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>
#include <executionGraph/common/Exception.hpp>

#ifdef __clang__
#    pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif

#define EXECGRAPHGUI_THROW(...) EXECGRAPH_THROW(__VA_ARGS__)
#define EXECGRAPHGUI_THROW_IF(condition, ...) EXECGRAPH_THROW_TYPE_IF(condition, executionGraph::Exception, __VA_ARGS__)
#define EXECGRAPHGUI_THROW_TYPE(Type, ...) EXECGRAPH_THROW_TYPE(Type, __VA_ARGS__) // Args: Type, ...
#define EXECGRAPHGUI_THROW_TYPE_IF(condition, Type, ...) EXECGRAPH_THROW_TYPE_IF(condition, Type, __VA_ARGS__) // Args: Type, ...

class InternalBackendError final : public executionGraph::Exception
{
public:
    InternalBackendError(const std::string& s)
        : executionGraph::Exception(s) {}
};

#    define EXECGRAPHGUI_THROW_BACKEND_ERROR_IF(condition, ...) EXECGRAPHGGUI_THROW_TYPE_IF(condition, InternalBackendError, __VA_ARGS__)
#    define EXECGRAPHGUI_THROW_BACKEND_ERROR(...) EXECGRAPHGGUI_THROW_TYPE(InternalBackendError, __VA_ARGS__)

#ifdef __clang__
#    pragma clang diagnostic pop
#endif

#endif
