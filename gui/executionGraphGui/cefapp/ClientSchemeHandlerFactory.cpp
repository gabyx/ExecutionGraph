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
#include <wrapper/cef_stream_resource_handler.h>
#include "cefapp/Loggers.hpp"
#include "cefapp/SchemeHandlerHelper.hpp"

namespace
{
    void debugTest(CefRefPtr<CefRequest> request)
    {
        std::string requestUrl = request->GetURL().ToString();
        CefURLParts urlParts;
        if(CefParseURL(request->GetURL(), urlParts))
        {
            // DEBUG TEST
            std::string temp = CefString(urlParts.path.str).ToString();
            auto path        = schemeHandlerHelper::splitPrefixFromPath(temp, "executionGraphBackend");

            if(!path)
            {
                EXECGRAPHGUI_APPLOG_ERROR("BackendSchemeHandlerFactory: requestUrl '{0}' failed!", requestUrl);
                return;
            }

            std::string requestId = path->filename();
            EXECGRAPHGUI_ASSERTMSG(!requestId.empty(), "Empty requestId in '{0}'!", requestUrl);

            // Printing the binary data ============
            CefRefPtr<CefPostData> postData = request->GetPostData();
            if(postData)
            {
                EXECGRAPHGUI_APPLOG_DEBUG("Received {0} post data elements.", postData->GetElementCount());
                CefPostData::ElementVector elements;
                postData->GetElements(elements);
                for(CefRefPtr<CefPostDataElement> element : elements)
                {
                    std::vector<uint8_t> buffer(element->GetBytesCount());
                    element->GetBytes(buffer.size(), static_cast<void*>(buffer.data()));
                    std::stringstream ss;
                    for(auto& byte : buffer)
                    {
                        ss << byte << ",";
                    }
                    EXECGRAPHGUI_APPLOG_DEBUG("Post Data Binary: '%s'", ss.str());
                }
            }
        }
    }

}  // namespace

CefRefPtr<CefResourceHandler> ClientSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                                                 CefRefPtr<CefFrame> frame,
                                                                 const CefString& scheme_name,
                                                                 CefRefPtr<CefRequest> request)
{
    std::string requestUrl = request->GetURL().ToString();
    CefURLParts urlParts;
    if(CefParseURL(request->GetURL(), urlParts))
    {
        debugTest(request);
        //! todo: why do we get here a urlParts.path.str as "//host/folderA/folderB"
        //! Shouldnt it be : "folderA/folderB".
        //! the host is somehow not parsed?: http://www.magpcss.org/ceforum/viewtopic.php?f=6&t=6048

        // e.g. "////host/folderA/folderB/file.ext"
        std::string temp = CefString(urlParts.path.str).ToString();
        auto filePath    = schemeHandlerHelper::splitPrefixFromPath(temp, m_pathPrefix);

        if(!filePath)
        {
            EXECGRAPHGUI_APPLOG_ERROR("ClientSchemeHandlerFactory: requestUrl '{0}' failed!", requestUrl);
            return nullptr;
        }

        filePath = m_folderPath / *filePath;
        EXECGRAPHGUI_APPLOG_DEBUG("ClientSchemeHandlerFactory: make stream for file: '{0}' ...", filePath->string());
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

            EXECGRAPHGUI_APPLOG_INFO("ClientSchemeHandlerFactory: requestUrl '{0}' handled!", requestUrl);
            return CefRefPtr<CefStreamResourceHandler>(new CefStreamResourceHandler(mimeType, fileStream));
        }
    }

    return nullptr;
}