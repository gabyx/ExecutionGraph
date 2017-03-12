// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_Common_Exception_hpp
#define ExecutionGraph_Common_Exception_hpp

#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>

namespace ExecutionGraph
{
class Exception : public std::runtime_error
{
public:
    Exception(const std::stringstream& ss) : std::runtime_error(ss.str()) {}

private:
};
}

#define EXEC_GRAPH_THROWEXCEPTION(message)                                                            \
    {                                                                                                 \
        std::stringstream ___s___;                                                                    \
        ___s___ << message << std::endl << " @ " << __FILE__ << " (" << __LINE__ << ")" << std::endl; \
        throw ExecutionGraph::Exception(___s___);                                                     \
    }

#define EXEC_GRAPH_THROWEXCEPTION_IF(condition, message) \
    if(condition)                                    \
    {                                                    \
        EXEC_GRAPH_THROWEXCEPTION(message);              \
    }

#endif
