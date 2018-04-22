// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Apr 22 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraph_serialization_ExecutionGraphSerializer_hpp
#    define executionGraph_serialization_ExecutionGraphSerializer_hpp

#    include <meta/meta.hpp>
#    include <string>

#    include "executionGraph/common/Exception.hpp"
#    include "executionGraph/common/TypeDefs.hpp"

namespace executionGraph
{
    template<typename GraphType>
    class ExecutionGraphSerializer
    {
    public:
        using GraphType = GraphType;

        ExecutionGraphSerializer(const GraphType& graph)
            : m_graph(graph)
        {}
        ~ExecutionGraphSerializer() = default;
    }

}  // namespace executionGraph