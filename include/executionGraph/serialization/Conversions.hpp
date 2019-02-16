// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jan 27 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#include <string_view>
#include <rttr/string_view.h>
#include "executionGraph/common/Platform.hpp"

namespace rttr
{
    rttr::string_view EXECGRAPH_EXPORT toRttr(std::string_view s);
}  // namespace rttr