// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Thu Aug 02 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <flatbuffers/flatbuffers.h>
#include "executionGraphGui/common/BinaryPayload.hpp"
#include "executionGraphGui/common/DevFlags.hpp"
#include "executionGraphGui/common/RequestError.hpp"

//! Get the root of the flatbuffer payload.
template<typename MessageType, bool verifyBuffer = devFlags::verifyAllFlatbufferMessages>
auto getRootOfPayloadAndVerify(const BinaryPayload& payload)
{
    const auto& buffer = payload.buffer();
    if(verifyBuffer)
    {
        flatbuffers::Verifier v(buffer.data(), buffer.size());
        EXECGRAPHGUI_THROW_BAD_REQUEST_IF(!v.VerifyBuffer<MessageType>(),
                                          "Flatbuffer corrupt!");
    }
    return flatbuffers::GetRoot<MessageType>(buffer.data());
}
