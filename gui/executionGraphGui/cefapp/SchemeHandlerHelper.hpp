//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Fri Feb 09 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef cefapp_SchemeHandlerHelper_hpp
#define cefapp_SchemeHandlerHelper_hpp

#include <optional>
#include <string>
#include "executionGraph/common/FileSystem.hpp"

namespace schemeHandlerHelper
{
    std::optional<std::path> splitPrefixFromPath(const std::string& path, const std::path& prefix);
}

#endif