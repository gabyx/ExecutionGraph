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

#include "cefapp/BackendResourceHandler.hpp"
#include <cef_parser.h>
#include <executionGraph/common/Assert.hpp>
#include <wrapper/cef_helpers.h>
#include "cefapp/Loggers.hpp"

namespace
{
    // Printing the binary data ============
    void printPostData(CefRefPtr<CefRequest> request)
    {
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
                EXECGRAPHGUI_APPLOG_DEBUG("Post Data Binary: bytes: '{0}', data: '{1}'", element->GetBytesCount(), ss.str());
            }
        }
        else
        {
            EXECGRAPHGUI_APPLOG_WARN("Received no post data!");
        }
    }
}  // namespace

void BackendResourceHandler::Cancel()
{
    CEF_REQUIRE_IO_THREAD();
    EXECGRAPH_ASSERT(m_isUsed, "Handler should be used by now!");
    EXECGRAPHGUI_APPLOG_ERROR(
        "BackendResourceHandler id:{0} : "
        "cancelled!",
        getId().getName());

    // if from external this Handling can be cancelled
    // we need to properly wait for pending tasks in the UI-Thread
    // and after that call finish() and leave here!
    // todo

    finish();
}

void BackendResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
                                                int64& responseLength,
                                                CefString& redirectUrl)
{
    CEF_REQUIRE_IO_THREAD();
    EXECGRAPH_ASSERT(m_isUsed, "Handler should be used by now!");

    //response->Set
    response->SetMimeType("application/octet-stream");
    responseLength = -1;  // we dont know the length we are gonna response with
}
bool BackendResourceHandler::ProcessRequest(CefRefPtr<CefRequest> request,
                                            CefRefPtr<CefCallback> callback)
{
    CEF_REQUIRE_IO_THREAD();
    EXECGRAPH_ASSERT(m_isUsed, "Handler should be used by now!");

    if(!initRequest(request))
    {
        // we dont handle this request or an error occured
        finish();
        return false;
    }

    EXECGRAPHGUI_APPLOG_DEBUG("BackendResourceHandler:: Handling requestId: '{0}', query: '{1}'", m_requestId.string(), m_query);

    printPostData(request);
    finish();
    return true;

    // Setup thread-safe binary buffer for post data and response data.
    // todo

    // Post a task to the UI-Thread to handle the message in the message dispatcher
    // which serializes the data, processes the request, and serializes a reponse
    // if an error happens, the returned response contains no payload, but a well defined error message.
    // todo

    //

    callback->Continue();  // Response headers are available.
    return true;
}
bool BackendResourceHandler::ReadResponse(void* dataOut,
                                          int bytesToRead,
                                          int& bytesRead,
                                          CefRefPtr<CefCallback> callback)
{
    CEF_REQUIRE_IO_THREAD();
    // Handler the repsponse
    // todo

    // Response is finished, we returned all bytes to read, so
    // finish up and return false.
    finish();
    return false;
}

bool BackendResourceHandler::initRequest(CefRefPtr<CefRequest> request)
{
    CefString url = request->GetURL();
    CefURLParts urlParts;
    if(!CefParseURL(url, urlParts))
    {
        EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler: id: '{0}' : url '{1}': url parse failed!",
                                  getId().getName(),
                                  url.ToString());
        return false;
    }

    // Exctract requestId
    // e.g. m_requestId := "catergory/subcategory/command"
    m_requestId = executionGraph::splitLeadingSlashes(CefString(urlParts.path.str).ToString());

    if(m_requestId.empty())
    {
        EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler id: '{0}' : url '{1}': requestId extract failed!",
                                  getId().getName(),
                                  url.ToString());
        return false;
    }

    // Extract additional query
    m_query = CefString(urlParts.query.str).ToString();

    return true;
}

//! Finish handling the request: Reset everyhting and signal callback.
void BackendResourceHandler::finish()
{
    reset();
    setUsed(false);
    m_callbackFinished();
}

//! Reset all internal data, such that this resource handler can be used again.
void BackendResourceHandler::reset()
{
    m_requestId = "";
    m_query     = "";
}
