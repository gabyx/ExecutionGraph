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
#include <wrapper/cef_stream_resource_handler.h>

FileSchemeHandlerFactory::FileSchemeHandlerFactory(std::string folderPath, std::string urlPrefix)
    : m_folderPath(folderPath)
    , m_urlPrefix(urlPrefix)
{
}

CefRefPtr<CefResourceHandler> FileSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                                               CefRefPtr<CefFrame> frame,
                                                               const CefString& scheme_name,
                                                               CefRefPtr<CefRequest> request)
{
    std::string requestUrl = request->GetURL().ToString();
    CefURLParts urlParts;
    if(CefParseURL(request->GetURL(), urlParts))
    {
        CefString path(urlParts.path.str);
        std::string resourceName = path.ToString();
        while(resourceName.find("/") == 0)
        {
            resourceName = resourceName.substr(1);
        }
        while(resourceName.find("\\") == 0)
        {
            resourceName = resourceName.substr(1);
        }

        if(m_urlPrefix == "" || resourceName.compare(0, m_urlPrefix.size(), m_urlPrefix) == 0)
        {
            if(m_urlPrefix != "")
            {
                resourceName = resourceName.substr(resourceName.find(m_urlPrefix) + m_urlPrefix.length());
                while(resourceName.find("/") == 0)
                {
                    resourceName = resourceName.substr(1);
                }
                while(resourceName.find("\\") == 0)
                {
                    resourceName = resourceName.substr(1);
                }
            }
            CefString sFilePath(m_folderPath + "/" + resourceName);
            CefString sFileExtension(sFilePath.ToString().substr(sFilePath.ToString().find_last_of(".") + 1));

            CefRefPtr<CefStreamReader> fileStream = CefStreamReader::CreateForFile(sFilePath);
            if(fileStream != nullptr)
            {
                CefString sMimeType(CefGetMimeType(sFileExtension));
                //todo: Complete known mime times with web-font extensions
                if(sMimeType.empty())
                {
                    sMimeType = "font/" + sFileExtension.ToString();
                }
                return CefRefPtr<CefStreamResourceHandler>(new CefStreamResourceHandler(sMimeType, fileStream));
            }
        }
    }

    return nullptr;
}