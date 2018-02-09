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

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include "TestFunctions.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif

struct Message
{
    int x, y;
    float z;

    template<class Archive>
    void serialize(Archive& ar, std::uint32_t const version)
    {
        ar(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z));
    }
};
CEREAL_CLASS_VERSION(Message, 32);

MY_TEST(JsonTest, Test1)
{
    std::ostringstream os;
    cereal::JSONOutputArchive archive(os);

    Message myData{1, 3, 4.0};
    archive(CEREAL_NVP(myData));
    std::cout << os.str() << std::endl;
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
