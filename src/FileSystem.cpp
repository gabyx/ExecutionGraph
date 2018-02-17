//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Sat Feb 17 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#include "executionGraph/common/FileSystem.hpp"

namespace executionGraph
{
    //! Split all leading slashes from `path`.
    std::path splitLeadingSlashes(const std::path& path)
    {
        std::path res;
        auto itC = path.begin();
        if(itC != path.end())
        {
            if(*itC == "/")
            {
                ++itC;
            }
            while(itC != path.end())
            {
                res /= *itC++;
            }
        }
        return res;
    }

    //! Split the prefix `prefix` from the path `path`
    std::optional<std::path> splitPrefixFromPath(const std::path& path, const std::path& prefix)
    {
        // path e.g. "////host/folderA/folderB/file.ext"
        std::path tempPath = executionGraph::splitLeadingSlashes(path);

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
}  // namespace executionGraph