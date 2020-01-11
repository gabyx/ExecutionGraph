// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Jan 11 2020
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#include "executionGraph/nodes/LogicSocketBase.hpp"
#include <rttr/type>
#include "executionGraph/common/Exception.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraph/nodes/LogicNode.hpp"

namespace executionGraph
{
    namespace details
    {
        void throwSocketCast(bool assert,
                             const LogicSocketInputBase& socket,
                             const rttr::type& type)
        {
            EG_LOGTHROW_IF(assert,
                           "Casting input socket index '{0}' with type index '{1}' into type"
                           "'{2}' of node id '{3}' which is wrong!",
                           socket.index(),
                           socket.type().get_name(),
                           type.get_name(),
                           socket.parent().getId());
        }
        void throwSocketCast(bool assert,
                             const LogicSocketOutputBase& socket,
                             const rttr::type& type)
        {
            EG_LOGTHROW_IF(assert,
                           "Casting output socket index '{0}' with type index '{1}' into type"
                           "'{2}' of node id '{3}' which is wrong!",
                           socket.index(),
                           socket.type().get_name(),
                           type.get_name(),
                           socket.parent().getId());
        }
    }  // namespace details
}  // namespace executionGraph