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

#include "executionGraph/nodes/LogicNode.hpp"
#include "executionGraph/common/Exception.hpp"
#include "executionGraph/nodes/LogicSocket.hpp"

namespace executionGraph
{
    //! Get the number of input sockets which are connected to other nodes.
    IndexType LogicNode::connectedInputCount() const
    {
        IndexType count = 0;
        for(auto& socket : this->getInputs())
        {
            if(socket->getConnectionCount() > 0)
            {
                ++count;
            }
        }
        return count;
    }

    //! Get the number of output sockets which are connected to other nodes.
    IndexType LogicNode::connectedOutputCount() const
    {
        IndexType count = 0;
        for(auto& socket : this->getOutputs())
        {
            if(socket->getConnectionCount() > 0)
            {
                ++count;
            }
        }
        return count;
    }
}  // namespace executionGraph
