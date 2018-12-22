// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Thu Dec 13 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGui_server_MimeType_hpp
#define executionGraphGui_server_MimeType_hpp

#include <string_view>
#include <executionGraph/common/FileSystem.hpp>

// Return a reasonable mime type based on the extension of a file.
std::string_view getMimeType(const std::path& path);

#endif