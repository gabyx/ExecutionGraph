// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_common_TypeDefs_hpp
#define ExecutionGraph_common_TypeDefs_hpp

#include "ExecutionGraph/common/MyContainerTypeDefs.hpp"
#include "ExecutionGraph/common/MyMatrixTypeDefs.hpp"
#include "ExecutionGraph/common/Platform.hpp"
#include "ExecutionGraph/config/Config.hpp"

namespace executionGraph
{
struct GlobalConfigs
{
    using PREC = double;
};

#define EXEC_GRAPH_DEFINE_MATRIX_TYPES                \
    using PREC = ExecutionGraph::GlobalConfigs::PREC; \
    EXEC_GRAPH_DEFINE_MATRIX_TYPES_OF(PREC)           \
    EXEC_GRAPH_DEFINE_CONTAINER_TYPES
}
#endif
