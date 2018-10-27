// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Oct 27 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#include "executionGraph/nodes/LogicCommon.hpp"

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
        Description for a socket link.
        See `LogicNode` for more information.

        @date Sat Oct 27 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    struct SocketLinkDescription
    {
        NodeId m_outNodeId;
        SocketIndex m_outSocketIdx;
        NodeId m_inNodeId;
        SocketIndex m_inSocketIdx;
        bool m_isWriteLink;
    };
}  // namespace executionGraph