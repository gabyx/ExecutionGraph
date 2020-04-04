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
#include <memory>
#include <rttr/registration>
#include <rttr/type>
#include "TestFunctions.hpp"
#include "executionGraph/common/Factory.hpp"

using namespace executionGraph;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif

struct MyMessage2 final
{
    RTTR_ENABLE()
public:
    int a = 3;
    void fromJSON();
    void toJSON();
};

RTTR_REGISTRATION
{
    rttr::registration::class_<MyMessage2>("MyMessage2")
        .constructor<>()(
            rttr::policy::ctor::as_std_shared_ptr);
}

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
    static FunnyTable create(int a) { return {"int"}; }
};
struct CreatorB
{
    using Key = double;
    static FunnyTable create(int b) { return {"double"}; }
};
struct CreatorC
{
    using Key = float;
    static FunnyTable create(int c) { return {"float"}; }
};

template<typename T, typename K>
using comp = std::is_same<typename T::Key, K>;

EG_TEST(FactoryTest, StaticFactory)
{
    using CreatorList    = meta::list<CreatorA, CreatorB, CreatorC>;
    using MySuperFactory = StaticFactory<CreatorList>;

    // create at compile time
    auto a = MySuperFactory::create<double>(1);
    auto b = MySuperFactory::create<int>(2);
    auto c = MySuperFactory::create<float>(3);

    ASSERT_EQ(a.a, "double");
    ASSERT_EQ(b.a, "int");
    ASSERT_EQ(c.a, "float");

    // Create if you have a rtti
    auto d = MySuperFactory::create(rttr::type::get<double>(), 1);
    auto e = MySuperFactory::create(rttr::type::get<int>(), 2);
    auto f = MySuperFactory::create(rttr::type::get<float>(), 3);

    ASSERT_EQ(d->a, "double");
    ASSERT_EQ(e->a, "int");
    ASSERT_EQ(f->a, "float");

    // Messages
    using varT             = std::shared_ptr<MyMessage2>;
    rttr::variant instance = rttr::type::get_by_name("MyMessage2").create();
    ASSERT_TRUE(instance.is_type<varT>());
    varT r = instance.get_value<varT>();
    ASSERT_TRUE(r->a == 3);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
