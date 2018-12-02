//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Wed Jan 10 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraph_common_FileSystem_hpp
#define executionGraph_common_FileSystem_hpp

#include <optional>
#if __has_include(<filesystem>)
#    include <filesystem>
#else
#    include <experimental/filesystem>
#endif
#include "executionGraph/common/Platform.hpp"
#include "executionGraph/config/Config.hpp"

namespace std
{
#if !(__has_include(<filesystem>))
    namespace filesystem = std::experimental::filesystem;
#endif

    using path = std::filesystem::path;

}  // namespace std

namespace executionGraph
{
    EXECGRAPH_EXPORT std::path splitLeadingSlashes(const std::path& path);
    EXECGRAPH_EXPORT std::optional<std::path> splitPrefixFromPath(const std::path& path, const std::path& prefix);
}  // namespace executionGraph

#endif