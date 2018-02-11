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
#include "cefapp/Loggers.hpp"
#include "cefapp/SchemeHandlerHelper.hpp"

CefRefPtr<CefResourceHandler> FileSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                                               CefRefPtr<CefFrame> frame,
                                                               const CefString& scheme_name,
                                                               CefRefPtr<CefRequest> request)
{
    std::string requestUrl = request->GetURL().ToString();
    CefURLParts urlParts;
    if(CefParseURL(request->GetURL(), urlParts))
    {
        //! todo: why do we get here a urlParts.path.str as "//host/folderA/folderB"
        //! Shouldnt it be : "folderA/folderB".
        //! the host is somehow not parsed?: http://www.magpcss.org/ceforum/viewtopic.php?f=6&t=6048

        // e.g. "////host/folderA/folderB/file.ext"
        std::string temp = CefString(urlParts.path.str).ToString();
        auto filePath    = schemeHandlerHelper::splitPrefixFromPath(temp, m_pathPrefix);

        if(!filePath)
        {
            EXECGRAPHGUI_APPLOG_ERROR("FileSchemeHandlerFactory: requestUrl '{0}' failed!", requestUrl);
            return nullptr;
        }

        filePath = m_folderPath / *filePath;
        EXECGRAPHGUI_APPLOG_DEBUG("FileSchemeHandlerFactory: make stream for file: '{0}' ...", filePath->string());
        CefRefPtr<CefStreamReader> fileStream = CefStreamReader::CreateForFile(filePath->string());
        if(fileStream != nullptr)
        {
            // "ext"
            std::string fileExtension = filePath->extension().string().substr(1);
            CefString mimeType(CefGetMimeType(fileExtension));
            //todo: Complete known mime times with web-font extensions
            if(mimeType.empty())
            {
                mimeType = "font/" + fileExtension;
            }

            EXECGRAPHGUI_APPLOG_INFO("FileSchemeHandlerFactory: requestUrl '{0}' handled!", requestUrl);
            return CefRefPtr<CefStreamResourceHandler>(new CefStreamResourceHandler(mimeType, fileStream));
        }
    }

    return nullptr;
}