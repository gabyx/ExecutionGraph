// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#include "GRSF/logic/LogicSocket.hpp"
#include <typeinfo>
#include "GRSF/logic/LogicNode.hpp"

void LogicSocketBase::link(LogicSocketBase* fsock)
{
    if (!fsock)
    {
        GRSF_ERRORMSG("Socket to link to is null!");
    }

    if (!(this->m_type == fsock->m_type))
    {
        GRSF_ERRORMSG(" Types of sockets do not coincide: " << LogicTypes::getTypeName(this->m_type) << " and "
                                                            << LogicTypes::getTypeName(fsock->m_type)
                                                            << " for tool id: "
                                                            << this->m_parent->m_id
                                                            << " and "
                                                            << fsock->m_parent->m_id);
    }

    if (m_isInput)
    {
        if (m_from)
        {
            GRSF_ERRORMSG("Only one link for input socket");
        }
        if (fsock->m_isInput)
        {
            GRSF_ERRORMSG("Cannot link input to input");
        }

        m_from = fsock;
    }
    else
    {
        if (!fsock->m_isInput)
        {
            GRSF_ERRORMSG("Cannot link output to output");
        }
        if (std::find(m_to.begin(), m_to.end(), fsock) == m_to.end())
        {
            m_to.push_back(fsock);
        }
    }

    fsock->m_connected = m_connected = true;

    if (m_parent)
    {
        m_parent->setLinked();
        LogicNode* nd = fsock->getParent();
        if (nd)
        {
            nd->setLinked();
        }
        else
        {
            GRSF_ERRORMSG("This socket has no parent");
        }
    }
    else
    {
        GRSF_ERRORMSG("This socket has no parent");
    }
}
