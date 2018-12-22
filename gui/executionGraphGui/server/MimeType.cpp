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
#include "executionGraphGui/server/MimeType.hpp"
#include <string>
#include <string_view>
#include <unordered_map>

// Return a reasonable mime type based on the extension of a file.
std::string_view getMimeType(const std::path& path)
{
    static const std::unordered_map<std::string, std::string> mapping = {
        {".htm", "text/html"},
        {".html", "text/html"},
        {".php", "text/html"},
        {".css", "text/css"},
        {".txt", "text/plain"},
        {".js", "application/javascript"},
        {".json", "application/json"},
        {".xml", "application/xml"},
        {".swf", "application/x-shockwave-flash"},
        {".flv", "video/x-flv"},
        {".png", "image/png"},
        {".jpe", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".jpg", "image/jpeg"},
        {".bmp", "image/bmp"},
        {".gif", "image/gif"},
        {".ico", "image/vnd.microsoft.icon"},
        {".tiff", "image/tiff"},
        {".tif", "image/tiff"},
        {".svg", "image/svg+xml"},
        {".svgz", "image/svg+xml"}};

    auto it = mapping.find(path.extension().c_str());
    if(it != mapping.end())
    {
        return it->second;
    }
    return "application/text";
}
