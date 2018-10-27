// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Jun 06 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef tests_GraphGenerator_hpp
#define tests_GraphGenerator_hpp

#include <random>
#include <executionGraph/common/Assert.hpp>
#include <executionGraph/common/Log.hpp>
#include <executionGraph/common/TypeDefs.hpp>
#include <executionGraph/config/Config.hpp>
#include <executionGraph/graphs/ExecutionTree.hpp>
#include <executionGraph/nodes/LogicNode.hpp>
#include <executionGraph/nodes/LogicSocket.hpp>

//! Generates a random graph with nodes of type `NodeType`.
//! @return  Instance of GraphType
template<typename GraphType, typename NodeType>
std::unique_ptr<GraphType> createRandomTree(std::size_t nNodes,
                                            std::size_t seed,
                                            bool makeCycle = false,
                                            bool noSetup   = false)
{
    auto execTree = std::make_unique<GraphType>();

    std::mt19937 gen(seed);  //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(0, nNodes - 1);
    std::vector<std::unique_ptr<NodeType>> vec(nNodes);

    execTree->getDefaultOuputPool().template setDefaultValue<int>(2);
    for(int i = 0; i < nNodes; ++i)
    {
        vec[i] = std::make_unique<NodeType>(i);
        EXECGRAPH_LOG_TRACE_CONT(vec[i]->getId() << ",");
    }
    EXECGRAPH_LOG_TRACE_CONT(std::endl);

    // Links
    std::vector<int> idWithConnectionToZero;
    idWithConnectionToZero.assign(nNodes, false);
    idWithConnectionToZero[0] = true;

    for(int i = 1; i < nNodes; ++i)
    {
        // Make link from input 1
        int id = (dis(gen) / ((double)nNodes)) * (i - 1);
        //EXECGRAPH_LOG_TRACE(id << "-->" << i <<"[0]");
        vec[i]->setGetLink(*vec[id], 0, 0);

        if(idWithConnectionToZero[id])
        {
            idWithConnectionToZero[i] = true;
        }

        // Make link from input 2

        id = (dis(gen) / ((double)nNodes)) * (i - 1);
        //EXECGRAPH_LOG_TRACE(id << "-->" << i <<"[1]");
        vec[i]->setGetLink(*vec[id], 0, 1);

        if(idWithConnectionToZero[id])
        {
            idWithConnectionToZero[i] = true;
        }
    }

    if(makeCycle)
    {
        auto it = std::find(idWithConnectionToZero.rbegin()++, idWithConnectionToZero.rend(), true);
        // Make a cycle
        vec[*it]->addWriteLink(0, *vec[0], 0);
    }

    std::shuffle(vec.begin(), vec.end(), gen);

    for(int i = 0; i < nNodes; ++i)
    {
        execTree->addNode(std::move(vec[i]));
    }

    for(int i = 0; i < nNodes; ++i)
    {
        auto* node = execTree->getNode(i);

        // EXECGRAPH_LOG_TRACE("id: " << i << " has " << node->getConnectedInputCount() <<" connected inputs.");
        // EXECGRAPH_LOG_TRACE("id: " << i << " has " << node->getConnectedOutputCount() <<" connected output.");
        if(node->getConnectedInputCount() == 0)
        {
            execTree->setNodeClass(*node, GraphType::NodeClassification::InputNode);
        }

        if(node->getConnectedOutputCount() == 0)
        {
            execTree->setNodeClass(*node, GraphType::NodeClassification::OutputNode);
        }
    }

    if(!noSetup)
    {
        execTree->setup(true);
    }
    return execTree;
}

#endif
