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

#pragma once

#include <string>

namespace executionGraph
{
    //! A simple node description, describing a LogicNode.
    struct NodeTypeDescription
    {
        std::string m_type;           //!< Unique RTTI name of the node
        std::string m_name = m_type;  //!< Readable name of the node type.

        std::vector<std::string> inSocketNames;   //!< Default input socket names (if available).
        std::vector<std::string> outSocketNames;  //!< Default output socket names (if available).
    };
}  // namespace executionGraph
