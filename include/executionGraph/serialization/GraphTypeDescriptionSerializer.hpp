// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Aug 11 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraph_serializer_GraphTypeDescriptionSerializer_hpp
#define executionGraph_serializer_GraphTypeDescriptionSerializer_hpp

#include <flatbuffers/flatbuffers.h>
#include "executionGraph/common/Platform.hpp"
#include "executionGraph/serialization/GraphTypeDescription.hpp"
#include "executionGraph/serialization/schemas/cpp/GraphTypeDescription_generated.h"

namespace executionGraph
{
    class EXECGRAPH_EXPORT GraphTypeDescriptionSerializer
    {
    public:
        static flatbuffers::Offset<serialization::GraphTypeDescription> write(flatbuffers::FlatBufferBuilder& builder,
                                                                              const GraphTypeDescription& graphDescription);
    };
}  // namespace executionGraph

#endif