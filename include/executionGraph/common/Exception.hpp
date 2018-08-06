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
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif

namespace executionGraph
{
    namespace details
    {
        template<typename Type, typename Message, typename... Args>
        void throwException(Message&& message, Args&&... args)
        {
            if constexpr(sizeof...(Args) > 0)
            {
                throw Type(fmt::format(message, std::forward<Args>(args)...));
            }
            else
            {
                throw Type(message);
            }
        }
    }  // namespace details
}  // namespace executionGraph

#define EXECGRAPH_THROW_TYPE(Type, ...) executionGraph::details::throwException<Type>(__VA_ARGS__);

#define EXECGRAPH_THROW_TYPE_IF(condition, Type, ...)               \
    if(condition)                                                   \
    {                                                               \
        executionGraph::details::throwException<Type>(__VA_ARGS__); \
    }

#define EXECGRAPH_THROW(...) EXECGRAPH_THROW_TYPE(executionGraph::Exception, __VA_ARGS__)
#define EXECGRAPH_THROW_IF(condition, ...) EXECGRAPH_THROW_TYPE_IF(condition, executionGraph::Exception, __VA_ARGS__)

namespace executionGraph
{
    class Exception : public std::runtime_error
    {
    public:
        Exception(const std::string& s)
            : std::runtime_error(s) {}
        // we dont need a virtual dtor, the std destroys the exception correctly.
        // https://stackoverflow.com/questions/28353708/exception-with-non-virtual-destructor-c
    };

    class NodeConnectionException final : public Exception
    {
    public:
        NodeConnectionException(const std::string& s)
            : Exception(s) {}
    };

    class ExecutionGraphCycleException final : public Exception
    {
    public:
        ExecutionGraphCycleException(const std::string& s)
            : Exception(s) {}
    };

    class BadSocketCastException final : public Exception
    {
    public:
        BadSocketCastException(const std::string& s)
            : Exception(s) {}
    };
}  // namespace executionGraph

#    ifdef __clang__
#        pragma clang diagnostic pop
#    endif

#endif
