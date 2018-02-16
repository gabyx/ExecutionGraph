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
    //! I tried here to register "backend" and "client" as in the minial
    //! example https://github.com/chromiumembedded/cef-project/blob/185e8338bfcb36d98f57797078a671c7c239766c/examples/scheme_handler/app_browser_impl.cc#L41
    //! which somehow does not work. even client:// does not get through anymore??
    inline std::vector<CefString> getCustomSchemes() { return {"client"}; }
    void registerCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar);
}  // namespace schemeHandlerHelper

#endif