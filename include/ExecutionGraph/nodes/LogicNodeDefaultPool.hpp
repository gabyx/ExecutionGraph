// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_nodes_LogicNodeDefaultPool_hpp
#define ExecutionGraph_nodes_LogicNodeDefaultPool_hpp

#include "ExecutionGraph/nodes/LogicNode.hpp"
#include "ExecutionGraph/nodes/LogicSocket.hpp"

namespace executionGraph
{
    //! Provides a pool of default output sockets for the configuration `TConfig`.
    template<typename TConfig>
    class LogicNodeDefaultPool final : public TConfig::NodeBaseType
    {
        public:
        EXEC_GRAPH_TYPEDEF_CONFIG(TConfig);
        
        LogicNodeDefaultPool()
        {
            // Add a ouput socket with a default-initialized value.
            auto add = [&](auto&& type) {
                using DataType = decltype(type);
                this->template addISock<DataType>(DataType{});
            };
            meta::for_each(SocketTypes{}, add);
        }
    };
};

#endif
