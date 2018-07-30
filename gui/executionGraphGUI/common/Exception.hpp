// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jul 29 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGUI_common_Exception_hpp
#define executionGraphGUI_common_Exception_hpp

#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>

#include <executionGraph/common/Exception.hpp>

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif

class InternalBackendError final : public executionGraph::Exception
{
public:
    InternalBackendError(const std::stringstream& ss)
        : executionGraph::Exception(ss) {}
};

#    define EXECGRAPHGUI_THROW_EXCEPTION_TYPE(message, type)                      \
        {                                                                         \
            std::stringstream ___s___;                                            \
            ___s___ << message << std::endl                                       \
                    << " @ " << __FILE__ << " (" << __LINE__ << ")" << std::endl; \
            throw type(___s___);                                                  \
        }

#    define EXECGRAPHGUI_THROW_EXCEPTION_TYPE_IF(condition, message, type) \
        if(condition)                                                      \
        {                                                                  \
            EXECGRAPHGUI_THROW_EXCEPTION_TYPE(message, type);              \
        }

#    define EXECGRAPHGUI_THROW_EXCEPTION(message) EXECGRAPHGUI_THROW_EXCEPTION_TYPE(message, executionGraph::Exception)
#    define EXECGRAPHGUI_THROW_EXCEPTION_IF(condition, message) EXECGRAPHGUI_THROW_EXCEPTION_TYPE_IF(condition, message, executionGraph::Exception)

#    ifdef __clang__
#        pragma clang diagnostic pop
#    endif

#endif
