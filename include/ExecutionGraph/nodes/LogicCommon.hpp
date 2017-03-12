// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_nodes_LogicCommon_hpp
#define ExecutionGraph_nodes_LogicCommon_hpp

#include <vector>

#include "ExecutionGraph/common/TypeDefs.hpp"
#include "ExecutionGraph/nodes/LogicSocketDefaultTypes.hpp"

namespace ExecutionGraph
{
// Forward declarations
template<typename TConfig>
class LogicNode;
template<typename TConfig>
class LogicSocketBase;
template<typename TConfig>
class LogicSocketInputBase;
template<typename TConfig>
class LogicSocketOutputBase;
template<typename T, typename TConfig>
class LogicSocketInput;
template<typename T, typename TConfig>
class LogicSocketOutput;

template<typename TSocketTypes = SocketDefaultTypes>
struct GeneralConfig
{
    using SocketTypes  = TSocketTypes;
    using NodeBaseType = LogicNode<GeneralConfig>;

    using SocketInputBaseType  = LogicSocketInputBase<GeneralConfig>;
    using SocketOutputBaseType = LogicSocketOutputBase<GeneralConfig>;

    template<typename T>
    using SocketInputType = LogicSocketInput<T, GeneralConfig>;
    template<typename T>
    using SocketOutputType = LogicSocketOutput<T, GeneralConfig>;
};

#define EXEC_GRAPH_TYPDEF_CONFIG(__CONFIG__)                              \
    using Config               = __CONFIG__;                              \
    using SocketTypes          = typename Config::SocketTypes;            \
    using NodeBaseType         = typename Config::NodeBaseType;           \
    using SocketInputBaseType  = typename Config::SocketInputBaseType;    \
    using SocketOutputBaseType = typename Config::SocketOutputBaseType;   \
    template<typename T>                                                  \
    using SocketInputType = typename Config::template SocketInputType<T>; \
    template<typename T>                                                  \
    using SocketOutputType = typename Config::template SocketOutputType<T>

}  // namespace

#endif
