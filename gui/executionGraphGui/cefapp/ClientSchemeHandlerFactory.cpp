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
#include "ClientSchemeHandlerFactory.hpp"
#include <cef_parser.h>
#include <wrapper/cef_helpers.h>
#include <wrapper/cef_stream_resource_handler.h>
#include "executionGraphGui/cefapp/SchemeHandlerHelper.hpp"
#include "executionGraphGui/common/Loggers.hpp"

CefRefPtr<CefResourceHandler> ClientSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                                                 CefRefPtr<CefFrame> frame,
                                                                 const CefString& scheme_name,
                                                                 CefRefPtr<CefRequest> request)
{
    CEF_REQUIRE_IO_THREAD();

    CefString url = request->GetURL();

    EGGUI_APPLOG_DEBUG("ClientSchemeHandlerFactory: handling url: '{0}' ...",
                              url.ToString());
    CefURLParts urlParts;
    if(CefParseURL(url, urlParts))
    {
        // e.g. temp := "/folderA/folderB/file.ext"
        std::path filePath = m_folderPath / CefString(urlParts.path.str).ToString();
        if(!std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))
        {
            filePath = m_folderPath / "index.html";
            EGGUI_APPLOG_WARN("ClientSchemeHandlerFactory: url '{0}' mapped to index: '{1}'",
                                     url.ToString(),
                                     filePath.string());
        }

        EGGUI_APPLOG_DEBUG("ClientSchemeHandlerFactory: make stream for file: '{0}' ...",
                                  filePath.string());
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

            EGGUI_APPLOG_INFO("ClientSchemeHandlerFactory: url '{0}' handled!", url.ToString());
            return CefRefPtr<CefStreamResourceHandler>(new CefStreamResourceHandler(mimeType, fileStream));
        }
    }

    EGGUI_APPLOG_DEBUG("ClientSchemeHandlerFactory: url '{0}' not handled!", url.ToString());
    return nullptr;
}