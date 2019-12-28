// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Sep 01 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include "executionGraphGui/common/Loggers.hpp"

// Define some asserts
#define EG_STRINGIFY(x) #x
#define EG_TOSTRING(x) EG_STRINGIFY(x)

#ifdef NDEBUG
#    define EGGUI_ASSERT(condition, ...)                                                    \
        {                                                                                          \
            if(!(condition))                                                                       \
            {                                                                                      \
                EGGUI_APPLOG_FATAL(__VA_ARGS__);                                            \
                EGGUI_THROW("Exception: @ " __FILE__ "(" EG_TOSTRING(__LINE__) ")"); \
            }                                                                                      \
        }

#    define EGGUI_VERIFY(condition, ...) EGGUI_ASSERT(condition, __VA_ARGS__)
#else
#    define EGGUI_ASSERT(condition, ...)         \
        {                                               \
            if(!(condition))                            \
            {                                           \
                EGGUI_APPLOG_FATAL(__VA_ARGS__); \
            }                                           \
        }
#    define EGGUI_VERIFY(condition, ...) EGGUI_ASSERT(condition, __VA_ARGS__)
#endif
