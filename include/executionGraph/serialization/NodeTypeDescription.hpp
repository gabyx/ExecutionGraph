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

#ifndef executionGraph_serialization_NodeTypeDescription_hpp
#define executionGraph_serialization_NodeTypeDescription_hpp

#include <string>

//! A simple node description, describing a LogicNode.
struct NodeTypeDescription
{
    NodeTypeDescription(const std::string& rtti)
        : m_rtti(rtti), m_name(m_rtti)
    {}
    NodeTypeDescription(const std::string& rtti, const std::string& name)
        : m_rtti(rtti), m_name(name)
    {}

    std::string m_rtti;  //!< The unique RTTI name of the node
    std::string m_name;  //!< The readable name of the node.
};

#endif