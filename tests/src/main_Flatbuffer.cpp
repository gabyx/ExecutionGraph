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

//#include <cereal/archives/json.hpp>
//#include <cereal/cereal.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include "TestFunctions.hpp"

#include <flatbuffers/flatbuffers.h>
#include "../files/testbuffer_generated.h"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif

MY_TEST(FlatBuffer, Test1)
{
    flatbuffers::FlatBufferBuilder builder;
    {
        testbuffer::Vec3 vec(1, 3, 4);
        auto testBuffer = testbuffer::CreateTestBufferDirect(builder, &vec, "asd", true);
        builder.Finish(testBuffer);
    }
    uint8_t* buf     = builder.GetBufferPointer();
    std::size_t size = builder.GetSize();
    std::vector<uint8_t> file(size);
    std::memcpy(file.data(), buf, size);

    // Reading back.
    auto readTestBuffer = testbuffer::GetTestBuffer(file.data());
    ASSERT_EQ(readTestBuffer->used(), true) << " Wupi, wrong serialization!";
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
