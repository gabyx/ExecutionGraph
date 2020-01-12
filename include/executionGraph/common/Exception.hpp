// ========================================================================================
// Execution Graph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#pragma once

#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "executionGraph/common/Log.hpp"
#include "executionGraph/common/Macros.hpp"

namespace executionGraph
{
    namespace details
    {
        template<typename Type, typename Message, typename... Args>
        inline void throwException(Message&& message, Args&&... args) noexcept(false)
        {
            if constexpr(sizeof...(Args) > 0)
            {
                throw Type(fmt::format(message, std::forward<Args>(args)...));
            }
            else
            {
                throw Type{message};
            }
        }
        template<typename Type>
        inline void throwException() noexcept(false)
        {
            throw Type{};
        }
    }  // namespace details
}  // namespace executionGraph

#define EG_THROW_TYPE(Type, ...) executionGraph::details::throwException<Type>(__VA_ARGS__)

#define EG_THROW_TYPE_IF(condition, Type, ...)                      \
    if(condition)                                                   \
    {                                                               \
        executionGraph::details::throwException<Type>(__VA_ARGS__); \
    }                                                               \
    ASSERT_SEMICOLON

#define EG_THROW(...) EG_THROW_TYPE(executionGraph::Exception, __VA_ARGS__)
#define EG_THROW_IF(condition, ...) EG_THROW_TYPE_IF(condition, executionGraph::Exception, __VA_ARGS__)
#define EG_LOGTHROW_IF(condition, ...)           \
    if(condition)                                \
    {                                            \
        EG_LOG_ERROR("{0} : \n{1}\n@ {2} [{3}]", \
                     #condition,                 \
                     fmt::format(__VA_ARGS__),   \
                     __FILE__,                   \
                     __LINE__);                  \
        EG_THROW(__VA_ARGS__);                   \
    }                                            \
    ASSERT_SEMICOLON

namespace executionGraph
{
    //! Special exception to denote a fatal error.
    class ExceptionFatal : public std::runtime_error
    {
    public:
        ExceptionFatal(const std::string& s)
            : std::runtime_error(s) {}
    };

    //! Base class for all exceptions.
    class Exception : public std::runtime_error
    {
    public:
        Exception(const std::string& s)
            : std::runtime_error(s) {}
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
