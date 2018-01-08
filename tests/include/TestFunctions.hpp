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
#ifndef TestFunctions_hpp
#define TestFunctions_hpp

#include <gtest/gtest.h>
#include <random>

#include "ExecutionGraph/common/Asserts.hpp"
#include "ExecutionGraph/common/SfinaeMacros.hpp"
#include "ExecutionGraph/common/TypeDefs.hpp"
#include "ExecutionGraph/config/Config.hpp"

#define MY_TEST(name1, name2) TEST(name1, name2)

#define MY_TEST_RANDOM_STUFF(name)                                                        \
    std::string testName = #name;                                                         \
    auto seed            = hashString(#name);                                             \
    EXECGRAPH_LOG_TRACE("Seed for this test: " << seed);                                  \
    ExecutionGraph::RandomGenerators::DefaultRandomGen rng(seed);                         \
    ExecutionGraph::RandomGenerators::DefaultUniformRealDistribution<PREC> uni(0.0, 1.0); \
    auto f = [&](PREC) { return uni(rng); };

#endif
