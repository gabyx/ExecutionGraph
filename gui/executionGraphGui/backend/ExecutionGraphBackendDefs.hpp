// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Fri Jul 06 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGui_backend_ExecutionGraphBackendDefs_hpp
#define executionGraphGui_backend_ExecutionGraphBackendDefs_hpp

#include <executionGraph/nodes/LogicCommon.hpp>
#include <meta/meta.hpp>
#include <rttr/type>
#include "backend/nodes/DummyNode.hpp"
#include "backend/nodes/NodeTypeDescription.hpp"
#include "backend/nodes/SocketTypeDescription.hpp"

template<typename TConfig>
class ExecutionGraphBackendDefs;

namespace details
{
    //! Return a set of rtti strings.
    template<typename T>
    struct MakeRTTIs;

    //! Spezialization for `meta::list<...>`.
    template<typename... NodeType>
    struct MakeRTTIs<meta::list<NodeType...>>
    {
        template<typename T>
        static std::vector<T> eval()
        {
            return {{T{rttr::type::get<NodeType>().get_name().to_string()}...}};
        }
    };

    template<typename T>
    struct RegisterAllRTTR;

    template<typename... NodeType>
    struct RegisterAllRTTR<meta::list<NodeType...>>
    {
        static void eval(const std::string& rttrPostfix)
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
            auto l = {(typename NodeType::AutoRegisterRTTR(rttrPostfix), 0)...};
#pragma clang diagnostic pop
        }
    };

}  // namespace details

//! Definitions for default configuration
template<>
class ExecutionGraphBackendDefs<executionGraph::GeneralConfig<>>
{
public:
    using Config = executionGraph::GeneralConfig<>;
    //! All Nodes
    using Nodes                         = meta::list<DummyNode<Config>>;
    static const std::size_t nNodeTypes = meta::size<Nodes>::value;

private:
    static std::vector<NodeTypeDescription> initAllNodeTypeDescription()
    {
        // Register all types
        details::RegisterAllRTTR<Nodes>::eval("DefaultGraph");

        // All nodes (here fixed number, to not make it compile)
        const std::array<std::string, 1> nodeNames = {"DummyNode"};
        static_assert(nNodeTypes == nodeNames.size(), "You need a name for each type in NodeTypes");

        // All rtti names
        const auto nodeRTTIs = details::MakeRTTIs<Nodes>::eval<std::string>();

        std::vector<NodeTypeDescription> description;
        for(std::size_t idx = 0; idx < nodeNames.size(); ++idx)
        {
            description.emplace_back(NodeTypeDescription{nodeNames[idx], nodeRTTIs[idx]});
        }
        return description;
    }

    static std::vector<SocketTypeDescription> initAllSocketTypeDescription()
    {
        return details::MakeRTTIs<typename Config::SocketTypes>::eval<SocketTypeDescription>();
    }

public:
    //! The static node description for this default configuration
    static const std::vector<NodeTypeDescription> NodeTypeDescriptions;
    //! The static socket description for this default configuration
    static const std::vector<SocketTypeDescription> SocketTypeDescriptions;
};

//! Static definitions
const std::vector<NodeTypeDescription> ExecutionGraphBackendDefs<executionGraph::GeneralConfig<>>::NodeTypeDescriptions =
    ExecutionGraphBackendDefs<executionGraph::GeneralConfig<>>::initAllNodeTypeDescription();

const std::vector<SocketTypeDescription> ExecutionGraphBackendDefs<executionGraph::GeneralConfig<>>::SocketTypeDescriptions =
    ExecutionGraphBackendDefs<executionGraph::GeneralConfig<>>::initAllSocketTypeDescription();

#endif