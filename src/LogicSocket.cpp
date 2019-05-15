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

#include "executionGraph/nodes/LogicSocket.hpp"

namespace executionGraph
{
    LogicSocketInputBase::~LogicSocketInputBase()
    {
        EXECGRAPH_LOG_TRACE("Destructor: LogicSocketInputBase: index: '{0}', parent: '{1}'",
                            this->getIndex(),
                            fmt::ptr(&this->parent()));

        // Reset Get-Link
        removeGetLink();

        // Reset all Write-Links
        for(auto* outputSocket : m_writingParents)
        {
            outputSocket->template removeWriteLink<false>(*this);
        }
    }

    //! Set the Get-Link to an output socket.
    void LogicSocketInputBase::setGetLink(LogicSocketOutputBase& outputSocket)
    {
        EXECGRAPH_THROW_TYPE_IF(outputSocket.parent().getId() == this->parent().getId(),
                                NodeConnectionException,
                                "No Get-Link connection to our output slot! (node id: '{0}')",
                                this->parent().getId());

        EXECGRAPH_THROW_TYPE_IF(this->type() != outputSocket.type(),
                                NodeConnectionException,
                                "Output socket index: '{0}' of node id: '{1}' has not the same type as "
                                "input socket index: '{2}' of node id: '{3}'!",
                                outputSocket.getIndex(),
                                outputSocket.parent().getId(),
                                this->getIndex(),
                                this->parent().getId());

        EXECGRAPH_THROW_TYPE_IF(m_writingParents.find(&outputSocket) != m_writingParents.end(),
                                NodeConnectionException,
                                "Cannot add Get-Link from input socket index: '{0}' of node id: '{1}' to "
                                "output socket index '{2}' of node id: '{3}' because output already has a "
                                "Write-Link to this input!",
                                this->getIndex(),
                                this->parent().getId(),
                                outputSocket.getIndex(),
                                outputSocket.parent().getId());

        // Remove Get-Link (if existing)
        removeGetLink();

        m_getFrom = &outputSocket;
        m_data    = outputSocket.m_data;  // Set data pointer of this input socket.
        outputSocket.m_getterChilds.emplace(this);
    }

    //! Callback when the output socket has removed its Write-Link
    //! to this input socket.
    void LogicSocketInputBase::onRemoveWritter(LogicSocketOutputBase& outputSocket)
    {
        m_writingParents.erase(&outputSocket);

        // If the data pointer points to this output socket try
        // to reroute to the Get-Link if possible.
        if(m_data == outputSocket.m_data)
        {
            m_data = hasGetLink() ? m_getFrom->m_data : nullptr;
        }
    }

    LogicSocketOutputBase::~LogicSocketOutputBase()
    {
        EXECGRAPH_LOG_TRACE("Destructor: LogicSocketOutputBase: index: '{0}', parent: '{1}'",
                            this->getIndex(),
                            fmt::ptr(&this->parent()));

        removeWriteLinks();

        // Reset data address in all input sockets.
        for(auto* inSocket : m_getterChilds)
        {
            inSocket->template removeGetLink<false>();
        }
    }

    //! Add a Write-Link to an input socket `inputSocket`.
    void LogicSocketOutputBase::addWriteLink(LogicSocketInputBase& inputSocket)
    {
        EXECGRAPH_THROW_TYPE_IF(inputSocket.parent().getId() == this->parent().getId(),
                                NodeConnectionException,
                                "No Write-Link connection to our input slot! (node id: '{0}')",
                                this->parent().getId());

        EXECGRAPH_THROW_TYPE_IF(this->type() != inputSocket.type(),
                                NodeConnectionException,
                                "Output socket index: '{1}' of node id: '{2}' "
                                "has not the same type as input socket index '{3}'"
                                "of node id: '{4}'",
                                this->getIndex(),
                                this->parent().getId(),
                                inputSocket.getIndex(),
                                inputSocket.parent().getId());

        EXECGRAPH_THROW_TYPE_IF(m_getterChilds.find(&inputSocket) != m_getterChilds.end(),
                                NodeConnectionException,
                                "Cannot add Write-Link from output socket index: '{0}' of node id: '{1}' to "
                                "input socket index '{2} of node id: '{3}' because input "
                                "already has a Get-Link to this output!",
                                this->getIndex(),
                                this->parent().getId(),
                                inputSocket.getIndex(),
                                inputSocket.parent().getId());

        if(std::find(m_writeTo.begin(), m_writeTo.end(), &inputSocket) == m_writeTo.end())
        {
            m_writeTo.push_back(&inputSocket);
            inputSocket.m_writingParents.emplace(this);
        }
    }
}  // namespace executionGraph