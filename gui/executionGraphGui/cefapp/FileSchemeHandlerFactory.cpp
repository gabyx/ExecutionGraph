//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 08 2018
//!  @author Gabriel Nützi, <gnuetzi (at) gmail (døt) com>
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================
#include "FileSchemeHandlerFactory.hpp"
#include <cef_parser.h>
#include <iostream>
#include <wrapper/cef_stream_resource_handler.h>

CefRefPtr<CefResourceHandler> FileSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                                               CefRefPtr<CefFrame> frame,
                                                               const CefString& scheme_name,
                                                               CefRefPtr<CefRequest> request)
{
    std::string requestUrl = request->GetURL().ToString();
    CefURLParts urlParts;
    if(CefParseURL(request->GetURL(), urlParts))
    {
        // e.g. "/asd/abc/abc/abcs.ext"
        std::string temp = CefString(urlParts.path.str).ToString();
        auto itC         = temp.begin();
        while(itC != temp.end() && *itC == '/')
        {
            ++itC;
        }
        std::path url(itC, temp.end());
        std::cout << "FileSchemeHandlerFactory::Create " << url << std::endl;
        // e.g. url : "asd/abc/abc/abcs.ext"

        // Split urlPrefix from front (e.g "asd/abc/abc.ext")
        auto it        = url.begin();
        auto itEnd     = url.end();
        auto itPref    = m_urlPrefix.begin();
        auto itPrefEnd = m_urlPrefix.end();
        for(; it != itEnd && itPref != itPrefEnd; ++it, ++itPref)
        {
            if(*itPref != *it)
            {
                break;
            }
        }
        if(itPref != itPrefEnd)
        {
            // Could not split urlPrefix
            return nullptr;
        }

        // Make new filePath from "m_folderPath + rest"
        std::path filePath;
        while(it != itEnd)
        {
            filePath /= *it++;
        }
        if(filePath.empty())
        {
            return nullptr;
        }
        filePath = m_folderPath / filePath;
        std::cout << "filePath: '" << filePath << "'" << std::endl;
        CefRefPtr<CefStreamReader> fileStream = CefStreamReader::CreateForFile(filePath.string());
        if(fileStream != nullptr)
        {
            // "ext"
            std::string fileExtension = filePath.extension().string().substr(1);
            CefString mimeType(CefGetMimeType(fileExtension));
            //todo: Complete known mime times with web-font extensions
            if(mimeType.empty())
            {
                mimeType = "font/" + fileExtension;
            }
            return CefRefPtr<CefStreamResourceHandler>(new CefStreamResourceHandler(mimeType, fileStream));
        }
    }

    return nullptr;
}