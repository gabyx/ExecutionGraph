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


namespace executionGraphGui
{
    // Return a reasonable mime type based on the extension of a file.
    boost::beast::string_view mime_type(boost::beast::string_view path)
    {
        using boost::beast::iequals;
        auto const ext = [&path] {
            auto const pos = path.rfind(".");
            if(pos == boost::beast::string_view::npos)
                return boost::beast::string_view{};
            return path.substr(pos);
        }();
        if(iequals(ext, ".htm"))
            return "text/html";
        if(iequals(ext, ".html"))
            return "text/html";
        if(iequals(ext, ".php"))
            return "text/html";
        if(iequals(ext, ".css"))
            return "text/css";
        if(iequals(ext, ".txt"))
            return "text/plain";
        if(iequals(ext, ".js"))
            return "application/javascript";
        if(iequals(ext, ".json"))
            return "application/json";
        if(iequals(ext, ".xml"))
            return "application/xml";
        if(iequals(ext, ".swf"))
            return "application/x-shockwave-flash";
        if(iequals(ext, ".flv"))
            return "video/x-flv";
        if(iequals(ext, ".png"))
            return "image/png";
        if(iequals(ext, ".jpe"))
            return "image/jpeg";
        if(iequals(ext, ".jpeg"))
            return "image/jpeg";
        if(iequals(ext, ".jpg"))
            return "image/jpeg";
        if(iequals(ext, ".gif"))
            return "image/gif";
        if(iequals(ext, ".bmp"))
            return "image/bmp";
        if(iequals(ext, ".ico"))
            return "image/vnd.microsoft.icon";
        if(iequals(ext, ".tiff"))
            return "image/tiff";
        if(iequals(ext, ".tif"))
            return "image/tiff";
        if(iequals(ext, ".svg"))
            return "image/svg+xml";
        if(iequals(ext, ".svgz"))
            return "image/svg+xml";
        return "application/text";
    }
}  // namespace executionGraphGui