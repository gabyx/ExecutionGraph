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

#ifndef executionGraphGUI_backend_requestHandlers_RequestHandlerCommon_hpp
#define executionGraphGUI_backend_requestHandlers_RequestHandlerCommon_hpp

#include <flatbuffers/flatbuffers.h>
#include "executionGraphGUI/common/BinaryPayload.hpp"
#include "executionGraphGUI/common/DevFlags.hpp"
#include "executionGraphGUI/common/RequestError.hpp"

template<typename MessageType, bool verifyBuffer = devFlags::verifyAllFlatbufferMessages>
auto getRootOfPayloadAndVerify(const BinaryPayload& payload)
{
    const auto& buffer = payload.getBuffer();
    if(verifyBuffer)
    {
        flatbuffers::Verifier v(buffer.getData(), buffer.getSize());
        EXECGRAPHGUI_THROW_BAD_REQUEST_IF(!v.VerifyBuffer<MessageType>(),
                                          "Flatbuffer corrupt!");
    }
    return flatbuffers::GetRoot<MessageType>(buffer.getData());
}

#endif