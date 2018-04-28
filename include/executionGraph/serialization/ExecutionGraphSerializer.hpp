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
#define executionGraph_serialization_ExecutionGraphSerializer_hpp

#include <meta/meta.hpp>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "executionGraph/common/Exception.hpp"
#include "executionGraph/common/TypeDefs.hpp"

namespace executionGraph
{
    template<typename GraphType>
    class ExecutionGraphSerializer
    {
    public:
        using GraphType = GraphType;

        ExecutionGraphSerializer(GraphType& graph)
            : m_graph(graph)
        {}
        ~ExecutionGraphSerializer() = default;

    public:
        void load(const std::path& filePath) throw
        {
        }

        void store(const std::path& filePath, bool bOverwrite = false) throw;

    private:
        GraphType& m_graph;
    };

}  // namespace executionGraph

#endif