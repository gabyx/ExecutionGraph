// ========================================================================================
// Execution Graph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef executionGraph_Common_Exception_hpp
#define executionGraph_Common_Exception_hpp

#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif

namespace executionGraph
{
    class Exception : public std::runtime_error
    {
    public:
        Exception(const std::stringstream& ss)
            : std::runtime_error(ss.str()) {}
        // we dont need a virtual dtor, the std destroys the exception correctly.
        // https://stackoverflow.com/questions/28353708/exception-with-non-virtual-destructor-c
    };

    class NodeConnectionException final : public Exception
    {
    public:
        NodeConnectionException(const std::stringstream& ss)
            : Exception(ss) {}
    };

    class ExecutionGraphCycleException final : public Exception
    {
    public:
        ExecutionGraphCycleException(const std::stringstream& ss)
            : Exception(ss) {}
    };

    class BadSocketCastException final : public Exception
    {
    public:
        BadSocketCastException(const std::stringstream& ss)
            : Exception(ss) {}
    };
}  // namespace executionGraph

// clang-format off
#define EXECGRAPH_THROW_EXCEPTION_TYPE(message, type)                         \
    {                                                                         \
        std::stringstream ___s___;                                            \
        ___s___ << message << std::endl                                       \
                << " @ " << __FILE__ << " (" << __LINE__ << ")" << std::endl; \
        throw executionGraph::type(___s___);                                  \
    }

#define EXECGRAPH_THROW_EXCEPTION_TYPE_IF(condition, message, type) \
    if(condition)                                                   \
    {                                                               \
        EXECGRAPH_THROW_EXCEPTION_TYPE(message, type);              \
    }

#define EXECGRAPH_THROW_EXCEPTION(message) EXECGRAPH_THROW_EXCEPTION_TYPE(message, Exception)
#define EXECGRAPH_THROW_EXCEPTION_IF(condition, message) EXECGRAPH_THROW_EXCEPTION_TYPE_IF(condition, message, Exception)


#ifdef __clang__
#   pragma clang diagnostic pop
#endif
// clang-format on

#endif
