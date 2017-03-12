// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <nuetzig (at) imes (d0t) mavt (d0t) ethz
//  (døt) ch>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_Common_TypeDefs_hpp
#define ExecutionGraph_Common_TypeDefs_hpp

#include "ExecutionGraph/Config/Config.hpp"
#include "ExecutionGraph/Common/Platform.hpp"
#include "ExecutionGraph/Common/MyMatrixTypeDefs.hpp"
#include "ExecutionGraph/Common/MyContainerTypeDefs.hpp"

namespace ExecutionGraph
{
struct GlobalConfigs
{
    using PREC = double;
};

#define EXEC_GRAPH_DEFINE_MATRIX_TYPES            \
    using PREC = ExecutionGraph::GlobalConfigs::PREC; \
    EXEC_GRAPH_DEFINE_MATRIX_TYPES_OF(PREC) EXEC_GRAPH_DEFINE_CONTAINER_TYPES
}
#endif
