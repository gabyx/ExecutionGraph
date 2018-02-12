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

#include "cefapp/SchemeHandlerHelper.hpp"
#include <executionGraph/common/FileSystem.hpp>

namespace schemeHandlerHelper
{
    //! Split the prefix `prefix` from the path `path`
    std::optional<std::path> splitPrefixFromPath(const std::string& path, const std::path& prefix)
    {
        // path e.g. "////host/folderA/folderB/file.ext"

        // Split all leeding slashes
        auto itC = path.begin();
        while(itC != path.end() && *itC == '/')
        {
            ++itC;
        }
        std::path tempPath(itC, path.end());
        // e.g. tempPath : "host/folderA/folderB/file.ext""

        // Split pathPrefix from front (e.g prefix := "host/folderA")
        auto it        = tempPath.begin();
        auto itEnd     = tempPath.end();
        auto itPref    = prefix.begin();
        auto itPrefEnd = prefix.end();
        for(; it != itEnd && itPref != itPrefEnd; ++it, ++itPref)
        {
            if(*itPref != *it)
            {
                break;
            }
        }
        if(itPref != itPrefEnd)
        {
            // Could not split pathPrefix
            return {};
        }

        // Concat the tempPath together
        std::path filePath;
        while(it != itEnd)
        {
            filePath /= *it++;
        }

        return filePath;
    }

    //! Register all custom schemes for the application as standard and secure.
    //! Must be the same implementation in all processes.
    void registerCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
    {
        for(auto& scheme : getCustomSchemes())
        {
            registrar->AddCustomScheme(scheme,
                                       true /* is standart*/,
                                       false /* is local */,
                                       false /* is display_isolated */,
                                       false /* is secure */,
                                       false /* is cors enabled*/,
                                       false /* is_csp_bypassing*/);
        }
    }

}  // namespace schemeHandlerHelper