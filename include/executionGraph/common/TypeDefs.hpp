//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 08 2018
//!  @author Gabriel Nützi, <gnuetzi (at) gmail (døt) com>
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraph_common_TypeDefs_hpp
#define executionGraph_common_TypeDefs_hpp

#include "executionGraph/common/MyContainerTypeDefs.hpp"
#include "executionGraph/common/MyMatrixTypeDefs.hpp"
#include "executionGraph/common/Platform.hpp"
#include "executionGraph/config/Config.hpp"

namespace executionGraph
{
    struct GlobalConfigs
    {
        using PREC = double;
    };

#define EXECGRAPH_DEFINE_MATRIX_TYPES                 \
    using PREC = ExecutionGraph::GlobalConfigs::PREC; \
    EXECGRAPH_DEFINE_MATRIX_TYPES_OF(PREC)            \
    EXECGRAPH_DEFINE_CONTAINER_TYPES
}
#endif
