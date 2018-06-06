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
#ifndef tests_TestFunctions_hpp
#define tests_TestFunctions_hpp

#include <executionGraph/common/Assert.hpp>
#include <executionGraph/common/SfinaeMacros.hpp>
#include <executionGraph/common/TypeDefs.hpp>
#include <executionGraph/config/Config.hpp>
#include <gtest/gtest.h>
#include <random>

#define DEFINE_RANDOM_GENERATOR_FUNC(seed)                \
    std::mt19937 rng(seed);                               \
    std::uniform_real_distribution<double> uni(0.0, 1.0); \
    auto rand = [&]() { return uni(rng); };

#define MY_TEST(name1, name2) TEST(name1, name2)

std::size_t hashString(std::string s);

#endif
