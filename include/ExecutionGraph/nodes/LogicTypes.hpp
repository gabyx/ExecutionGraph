// ========================================================================================
//  GRSFramework
//  Copyright (C) 2016 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the GNU General Public License as
//  published by the Free Software Foundation; either version 3 of the License,
//  or (at your option) any later version. If a copy of the GPL was not distributed with
//  this file, you can obtain one at http://www.gnu.org/licenses/gpl-3.0.html.
// ========================================================================================

#ifndef GRSF_logic_LogicTypes_hpp
#define GRSF_logic_LogicTypes_hpp

#include <string>
#include <meta/meta.hpp>
#include <boost/filesystem.hpp>

#include "GRSF/common/TypeDefs.hpp"

namespace ExecutionGraph
{

using SocketTypesDefault = boost::mpl::vector<double,
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

#define LOGICSOCKET_CASE_SWITCH(N)                                \
    case N:                                                       \
    {                                                             \
        using Type = meta::at<SocketTypes, N>;                    \
        visitor(castToType<Type>());                              \
    }                                                             \
    break;

#define LOGICSOCKET_APPLY_VISITOR_SWITCH                                                             \
                                                                                                     \
    switch (this->m_type)                                                                            \
    {                                                                                                \
        LOGICSOCKET_CASE_SWITCH(0)                                                                   \
        LOGICSOCKET_CASE_SWITCH(1)                                                                   \
        LOGICSOCKET_CASE_SWITCH(2)                                                                   \
        LOGICSOCKET_CASE_SWITCH(3)                                                                   \
        LOGICSOCKET_CASE_SWITCH(4)                                                                   \
        LOGICSOCKET_CASE_SWITCH(5)                                                                   \
        LOGICSOCKET_CASE_SWITCH(6)                                                                   \
        LOGICSOCKET_CASE_SWITCH(7)                                                                   \
        LOGICSOCKET_CASE_SWITCH(8)                                                                   \
        LOGICSOCKET_CASE_SWITCH(9)                                                                   \
        LOGICSOCKET_CASE_SWITCH(10)                                                                  \
        LOGICSOCKET_CASE_SWITCH(11)                                                                  \
        LOGICSOCKET_CASE_SWITCH(12)                                                                  \
        LOGICSOCKET_CASE_SWITCH(13)                                                                  \
        LOGICSOCKET_CASE_SWITCH(14)                                                                  \
        LOGICSOCKET_CASE_SWITCH(15)                                                                  \
        LOGICSOCKET_CASE_SWITCH(16)                                                                  \
        LOGICSOCKET_CASE_SWITCH(17)                                                                  \
        LOGICSOCKET_CASE_SWITCH(18)                                                                  \
        LOGICSOCKET_CASE_SWITCH(19)                                                                  \
        default:                                                                                     \
            EXEC_GRAPH_ERRORMSG("TYPE: " << this->m_type << " not implemented in switch statement"); \
    };
};

#endif  // LogicTypes_hpp
