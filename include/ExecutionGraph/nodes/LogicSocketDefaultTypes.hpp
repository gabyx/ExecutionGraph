// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_logic_LogicTypes_hpp
#define ExecutionGraph_logic_LogicTypes_hpp

#include <meta/meta.hpp>
#include <string>

#include "ExecutionGraph/common/Exception.hpp"
#include "ExecutionGraph/common/TypeDefs.hpp"

namespace ExecutionGraph
{
using SocketDefaultTypes = meta::list<double,
                                      float,
                                      bool,
                                      char,
                                      short,
                                      int,
                                      long int,
                                      long long int,
                                      unsigned char,
                                      unsigned short,
                                      unsigned int,
                                      unsigned long int,
                                      unsigned long long int,
                                      std::string>;

#define LOGICSOCKET_CASE_SWITCH(SOCKET_TYPES, N) \
    case N:                                      \
    {                                            \
        using Type = meta::at<SOCKET_TYPES, N>;  \
        visitor(castToType<Type>());             \
    }                                            \
    break;

#define LOGICSOCKET_APPLY_VISITOR_SWITCH                                                                   \
                                                                                                           \
    switch (this->m_type)                                                                                  \
    {                                                                                                      \
        LOGICSOCKET_CASE_SWITCH(0)                                                                         \
        LOGICSOCKET_CASE_SWITCH(1)                                                                         \
        LOGICSOCKET_CASE_SWITCH(2)                                                                         \
        LOGICSOCKET_CASE_SWITCH(3)                                                                         \
        LOGICSOCKET_CASE_SWITCH(4)                                                                         \
        LOGICSOCKET_CASE_SWITCH(5)                                                                         \
        LOGICSOCKET_CASE_SWITCH(6)                                                                         \
        LOGICSOCKET_CASE_SWITCH(7)                                                                         \
        LOGICSOCKET_CASE_SWITCH(8)                                                                         \
        LOGICSOCKET_CASE_SWITCH(9)                                                                         \
        LOGICSOCKET_CASE_SWITCH(10)                                                                        \
        LOGICSOCKET_CASE_SWITCH(11)                                                                        \
        LOGICSOCKET_CASE_SWITCH(12)                                                                        \
        LOGICSOCKET_CASE_SWITCH(13)                                                                        \
        LOGICSOCKET_CASE_SWITCH(14)                                                                        \
        LOGICSOCKET_CASE_SWITCH(15)                                                                        \
        LOGICSOCKET_CASE_SWITCH(16)                                                                        \
        LOGICSOCKET_CASE_SWITCH(17)                                                                        \
        LOGICSOCKET_CASE_SWITCH(18)                                                                        \
        LOGICSOCKET_CASE_SWITCH(19)                                                                        \
        default:                                                                                           \
            EXEC_GRAPH_THROWEXCEPTION("TYPE: " << this->m_type << " not implemented in switch statement"); \
    };
};

#endif  // LogicTypes_hpp
