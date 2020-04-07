//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 08 2018
//!  @author Gabriel Nützi, <gnuetzi (at) gmail (døt) com>
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#pragma once

#include "executionGraph/common/Exception.hpp"
#include "executionGraph/common/Log.hpp"
#include "executionGraph/common/Macros.hpp"
#include "executionGraph/config/Config.hpp"

//! Some assert macro.
#ifdef NDEBUG
    #define EG_DEBUG_ONLY(code) ASSERT_SEMICOLON
    #define EG_ASSERT(condition, ...) ASSERT_SEMICOLON
    #define EG_VERIFY(condition, ...) \
        condition;                    \
        ASSERT_SEMICOLON
    #define EG_ASSERT_TYPE(condition, type, ...) ASSERT_SEMICOLON
#else
    // Debug!
    #define EG_DEBUG_ONLY(code) code
    #define EG_VERIFY(condition, ...) EG_ASSERT(condition, __VA_ARGS__)
    #define EG_ASSERT(condition, ...) EG_ASSERT_TYPE(condition, executionGraph::ExceptionFatal, __VA_ARGS__)
    #define EG_ASSERT_TYPE(condition, Type, ...)               \
        if(!(condition))                                       \
        {                                                      \
            EG_LOG_ERROR("{0} : \n{1}\n@ {2} [{3}]",           \
                         #condition,                           \
                         fmt::format(__VA_ARGS__),             \
                         __FILE__,                             \
                         __LINE__);                            \
            EG_THROW_TYPE_IF(!(condition), Type, __VA_ARGS__); \
        }                                                      \
        ASSERT_SEMICOLON
#endif

//! Some warning macro.
#define EG_WARN(condition, ...)                     \
    {                                               \
        if(!(condition))                            \
        {                                           \
            EG_LOG_WARN("{0} : \n{1}\n@ {2} [{3}]", \
                        #condition,                 \
                        fmt::format(__VA_ARGS__),   \
                        __FILE__,                   \
                        __LINE__);                  \
        }                                           \
    }                                               \
    ASSERT_SEMICOLON
