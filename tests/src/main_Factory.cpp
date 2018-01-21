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
#include "TestFunctions.hpp"
#include "executionGraph/common/Factory.hpp"

using namespace executionGraph;

struct FunnyTable
{
    FunnyTable(std::string a)
        : a(a)
    {
        std::cout << "created FunnyTable " << a << std::endl;
    }
    std::string a;
};

struct CreatorA
{
    using Key = int;
    static FunnyTable create() { return {"int"}; }
};
struct CreatorB
{
    using Key = double;
    static FunnyTable create() { return {"double"}; }
};
struct CreatorC
{
    using Key = float;
    static FunnyTable create() { return {"float"}; }
};

template<typename T, typename K>
using comp = std::is_same<typename T::Key, K>;

MY_TEST(FactoryTest, StaticFactory)
{
    using CreatorList    = meta::list<CreatorA, CreatorB, CreatorC>;
    using MySuperFactory = StaticFactory<CreatorList>;

    // create at compile time
    auto a = MySuperFactory::create<double>();
    auto b = MySuperFactory::create<int>();
    auto c = MySuperFactory::create<float>();

    ASSERT_EQ(a.a, "double");
    ASSERT_EQ(b.a, "int");
    ASSERT_EQ(c.a, "float");

    // Create if you have a rtti
    auto d = MySuperFactory::create(rttr::type::get<double>());
    auto e = MySuperFactory::create(rttr::type::get<int>());
    auto f = MySuperFactory::create(rttr::type::get<float>());

    ASSERT_EQ(d.a, "double");
    ASSERT_EQ(e.a, "int");
    ASSERT_EQ(f.a, "float");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
