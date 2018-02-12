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

#include <cef_scheme.h>
#include <executionGraph/common/FileSystem.hpp>
#include <optional>
#include <string>
#include <vector>

namespace schemeHandlerHelper
{
    std::optional<std::path> splitPrefixFromPath(const std::string& path, const std::path& prefix);

    //! Return all custom schemes which get registered in this application.
    inline std::vector<std::string> getCustomSchemes() { return {"client", "backend"}; }
    void registerCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar);
}  // namespace schemeHandlerHelper

#endif