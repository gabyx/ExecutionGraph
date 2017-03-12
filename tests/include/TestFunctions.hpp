// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <nuetzig (at) imes (d0t) mavt (d0t) ethz (døt) ch>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================
#ifndef TestFunctions_hpp
#define TestFunctions_hpp

#include <gtest/gtest.h>

#include "ExecutionGraph/config/Config.hpp"
#include "ExecutionGraph/common/Asserts.hpp"
#include "ExecutionGraph/common/SfinaeMacros.hpp"
#include "ExecutionGraph/common/TypeDefs.hpp"


#define MY_TEST(name1 , name2 ) TEST(name1, name2)

#define MY_TEST_RANDOM_STUFF(name) \
        std::string testName = #name ; \
        auto seed = hashString(#name); \
        std::cout << "Seed for this test: " << seed << std::endl; \
        ExecutionGraph::RandomGenerators::DefaultRandomGen rng(seed); \
        ExecutionGraph::RandomGenerators::DefaultUniformRealDistribution<PREC> uni(0.0,1.0); \
        auto f = [&](PREC) { return uni(rng); };

#endif
