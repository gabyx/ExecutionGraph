// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#include "GRSF/logic/LogicNode.hpp"
#include "GRSF/logic/LogicSocket.hpp"

LogicNode::LogicNode(unsigned int id) : m_id(id), m_hasLinks(false), m_priority(0)
{
    // m_sockets.assign(nSockets,nullptr);
}

LogicNode::~LogicNode()
{
    for (auto& socket : m_inputs)
    {
        delete socket;
    }

    for (auto& socket : m_outputs)
    {
        delete socket;
    }
}

LogicSocketBase* LogicNode::getISocket(unsigned int index)
{
    if (index < m_inputs.size())
        return m_inputs.at(index);
    return nullptr;
}

LogicSocketBase* LogicNode::getOSocket(unsigned int index)
{
    if (index < m_outputs.size())
        return m_outputs.at(index);
    return nullptr;
}

void LogicNode::makeGetLink(LogicNode* outN, unsigned int outS, LogicNode* inN, unsigned int inS)
{
    if (outS >= outN->getOutputs().size() || inS >= inN->getInputs().size())
    {
        GRSF_ERRORMSG("Wrong socket indices: outNode: " << outN->m_id << " outS: " << outS << " inNode: " << inN->m_id
                                                        << " inS: "
                                                        << inS)
    }

    inN->getISocket(inS)->link(outN->getOSocket(outS));
}

void LogicNode::makeWriteLink(LogicNode* outN, unsigned int outS, LogicNode* inN, unsigned int inS)
{
    if (outS >= outN->getOutputs().size() || inS >= inN->getInputs().size())
    {
        GRSF_ERRORMSG("Wrong socket indices: outNode: " << outN->m_id << " outS: " << outS << " inNode: " << inN->m_id
                                                        << " inS: "
                                                        << inS)
    }

    outN->getOSocket(outS)->link(inN->getISocket(inS));
}
