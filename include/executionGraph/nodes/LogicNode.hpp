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

#pragma once

#include <memory>
#include <vector>
#include "executionGraph/common/Assert.hpp"
#include "executionGraph/common/DemangleTypes.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"

namespace executionGraph
{
    class LogicSocketInputBase;
    class LogicSocketOutputBase;

    /**
 @brief The execution node (node) class which is the base class
        for every node in an execution graph.

        // clang-format off
        General Concept
                               +-------------+                                       +-------------+
                               | LogicNode A |                                       | LogicNode B |
                               |             |                                       |             |
                               |     +-------+-----+                           +-----+-----+       |
                               |     |  Out Socket |                           | In Socket |       |
                               |     |             |                           |           |       |
                               |     | m_writeTo[0]|@------( Write-Link )----> |           |       |
                               |     |             |                           |           |       |
                               |     |             |                           |           |       |
                               |     |             |                           |           |       |
                               |     |             | <-----(  Get-Link  )-----@| m_getFrom |       |
                               |     |             |                           |           |       |
                               |     |             |                           |           |       |
                               |     |   T m_data  |                           | T* m_data |       |
                               |     +-------+-----+                           +-----+-----+       |
                               |             |                                       |             |
                               +-------------+                                       +-------------+

                Function Behavior in Out Socket:  (this =  Out)  ++      Function Behavior in In Socket:  (this = In)
                ================================                 ||      ===============================
                                                                 ||
                -data():  gets this->m_data,                 ||      -data(): gets the value *this->m_data
                                                                 ||                   Exception if nullptr
                                                                 ||
                                                                 ||
                -setData():  set all Write-Links directly       ||      -setData(): sets internal this->m_data
                              (m_writeTo array) and set          ||
                              this->m_data also because this     ||
                              might have other                   ||
                              Get-Links                          ||
                                                                 ++
        // clang-format on
*/

    class LogicNode
    {
    public:
        EXECGRAPH_DEFINE_TYPES();

        using SocketInputListType  = std::vector<SocketPointer<LogicSocketInputBase>>;
        using SocketOutputListType = std::vector<SocketPointer<LogicSocketOutputBase>>;

    public:
        //! The basic constructor of a node.
        LogicNode(NodeId id = nodeIdInvalid)
            : m_id(id)
        {
        }

        LogicNode(const LogicNode&) = default;
        LogicNode(LogicNode&&)      = default;

        virtual ~LogicNode() = default;

        //! The reset function.
        virtual void reset() = 0;

        //! The main compute function of this execution node.
        virtual void compute() = 0;

        inline NodeId getId() const { return m_id; }
        inline void setId(NodeId id) { m_id = id; }

        //! Get the list of input sockets.
        inline const SocketInputListType& getInputs() const { return m_inputs; }
        inline SocketInputListType& getInputs() { return m_inputs; }
        //! Get the list of output sockets.
        inline const SocketOutputListType& getOutputs() const { return m_outputs; }
        inline SocketOutputListType& getOutputs() { return m_outputs; }

        IndexType connectedInputCount() const;
        IndexType connectedOutputCount() const;

    protected:
        NodeId m_id;                     //!< The id of the node.
        SocketInputListType m_inputs;    //!< The input sockets.
        SocketOutputListType m_outputs;  //!< The output sockets.
    };

