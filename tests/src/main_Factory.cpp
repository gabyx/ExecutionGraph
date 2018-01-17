//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Tue Jan 16 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#include <iostream>
#include "executionGraph/common/Factory.hpp"

using namespace executionGraph

    struct FunnyTable
{
    FunnyTable(int a)
    {
        std::cout << "created FunnyTable " << a << std::endl;
    }
};

struct CreatorA
{
    using Key                = int;
    static constexpr Key key = 1;
    static FunnyTable create(){return {key}};
};
struct CreatorB
{
    using Key                = int;
    static constexpr Key key = 2;
    static FunnyTable create(){return {key}};
};
struct CreatorC
{
    using Key                = int;
    static constexpr Key key = 3;
    static FunnyTable create(){return {key}};
};

int main(int argc, char** argv)
{
    using MySuperFactory = typename StaticFactory<CreatorA, CreatorB, CreatorC, CreatorC>;

    auto a = MySuperFactory::create(1);
    auto b = MySuperFactory::create(2);
    auto c = MySuperFactory::create(3);
}
