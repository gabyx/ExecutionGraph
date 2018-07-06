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

#ifndef executionGraphGui_backend_nodes_NodeTypeDescription_hpp
#define executionGraphGui_backend_nodes_NodeTypeDescription_hpp

//! A simple node description, describing a LogicNode.
struct NodeTypeDescription
{
    std::string m_name;
    std::string m_rtti;
};

//! A simple node description, describing a LogicSocket.
struct SocketTypeDescription
{
    std::string m_rtti;
};

#endif