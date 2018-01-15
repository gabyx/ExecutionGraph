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

#ifndef executionGraph_Common_Platform_hpp
#define executionGraph_Common_Platform_hpp

#include "executionGraph/config/Config.hpp"

#ifdef __CYGWIN__
#include "executionGraph/Common/CygwinPatch.hpp"
#endif

namespace executionGraph
{
#if(defined _WIN32) || (defined __CYGWIN__) || (defined WIN32)

// This macro is given to the compiler when building the library!
#ifdef ExecutionGraph_BUILD_LIBRARY

#pragma message(" Platform.hpp: Building library ...")

#ifdef __GNUC__
#define EXECGRAPH_EXPORT __attribute__((dllexport))
#else
#define EXECGRAPH_EXPORT __declspec(dllexport)  // Note: actually gcc seems to also supports this syntax.
#endif

#else

#ifdef __GNUC__
#define EXECGRAPH_EXPORT __attribute__((dllimport))
#else
#define EXECGRAPH_EXPORT __declspec(dllimport)  // Note: actually gcc seems to also supports this syntax.
#endif

#endif

#else

#ifdef ExecutionGraph_BUILD_LIBRARY

#pragma message(" Platform.hpp: Building library ...")

#if __GNUC__ >= 4 || __clang__
#define EXECGRAPH_EXPORT __attribute__((visibility("default")))
#else
#define EXECGRAPH_EXPORT
#warning "Unknown compiler: Exporting everything into library!"
#endif

#else
#define EXECGRAPH_EXPORT
#endif

#endif
}  // namespace executionGraph

#endif
