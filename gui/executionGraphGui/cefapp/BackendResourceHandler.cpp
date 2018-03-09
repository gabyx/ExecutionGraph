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
#include <array>
#include <cef_parser.h>
#include <chrono>
#include <executionGraph/common/Assert.hpp>
#include <thread>
#include <wrapper/cef_closure_task.h>
#include <wrapper/cef_helpers.h>
#include "cefapp/BinaryBuffer.hpp"
#include "cefapp/Loggers.hpp"
#include "cefapp/RequestCef.hpp"
#include "cefapp/ResponseCef.hpp"

namespace
{
    const std::array<uint8_t, 10> c_debugResponse = {'e', 'x', 'e', 'c', 'g', 'r', 'a', 'p', 'h', '!'};

    // void printPostData2(CefRefPtr<CefRequest> request)
    // {
    //     CefRefPtr<CefPostData> postData = request->GetPostData();
    //     if(postData)
    //     {
    //         EXECGRAPHGUI_APPLOG_DEBUG("Received {0} post data elements.", postData->GetElementCount());
    //         CefPostData::ElementVector elements;
    //         postData->GetElements(elements);
    //         for(CefRefPtr<CefPostDataElement> element : elements)
    //         {
    //             std::vector<uint8_t> buffer(element->GetBytesCount());
    //             element->GetBytes(buffer.size(), static_cast<void*>(buffer.data()));
    //             std::stringstream ss;
    //             for(auto& byte : buffer)
    //             {
    //                 ss << byte << ",";
    //             }
    //             EXECGRAPHGUI_APPLOG_DEBUG("Post Data Binary: bytes: '{0}', data: '{1}'", element->GetBytesCount(), ss.str());
    //         }
    //     }
    //     else
    //     {
    //         EXECGRAPHGUI_APPLOG_WARN("Received no post data!");
    //     }
    // }

    //! Printing the binary data
    void printPostData(const BinaryBuffer<BufferPool>& buffer)
    {
        std::stringstream ss;
        for(const uint8_t& byte : buffer)
        {
            ss << byte << ",";
        }
        EXECGRAPHGUI_APPLOG_DEBUG("PostData received: '{0}'", ss.str());
    }

    //! Wait some time in another other to simulate working.
    void debugWaitOnOtherThread(CefRefPtr<CefCallback> callback)
    {
        using namespace std::chrono_literals;

        EXECGRAPHGUI_APPLOG_DEBUG("Computing started [2sec] ...");
        std::this_thread::sleep_for(1.3s);
        EXECGRAPHGUI_APPLOG_DEBUG("Computing finished!");

        callback->Continue();  // Signal that response headers are here.
    }

    //! Read the post data `postData` and store it in the `BinaryBuffer`.
    template<typename RawAllocator>
    bool readPostData(CefRefPtr<CefPostData> postData,
                      const std::string& mimeType,
                      std::optional<RequestCef::Payload>& payload,
                      std::shared_ptr<RawAllocator> allocator)
    {
        if(mimeType.empty())
        {
            return false;
        }
        // Read post data
        if(postData && postData->GetElementCount() != 0)
        {
            CefPostData::ElementVector elements;
            postData->GetElements(elements);

            auto& element = elements.back();  // get the last post data element.

            if(element->GetBytesCount() == 0)
            {
                EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler: Received no bytes!");
                return false;  // dont continue
            }

            // Allocate BinaryBuffer
            BinaryBuffer<BufferPool> buffer(element->GetBytesCount(), allocator);
            element->GetBytes(buffer.getSize(), static_cast<void*>(buffer.getData()));
            EXECGRAPHGUI_APPLOG_DEBUG("BackendResourceHandler: Read last post data element: bytes: {0}.", element->GetBytesCount());
            EXECGRAPHGUI_LOGCODE_DEBUG(printPostData(buffer));
            payload = RequestCef::Payload{std::move(buffer), mimeType};
            return true;  // continue
        }

        EXECGRAPHGUI_APPLOG_WARN("BackendResourceHandler: Received no post data!");
        return false;  // dont continue
    }

}  // namespace

//! Cancel the request (see: http://magpcss.org/ceforum/apidocs3/projects/(default)/CefResourceHandler.html)
void BackendResourceHandler::Cancel()
{
    CEF_REQUIRE_IO_THREAD();
    EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler: id: '{0}' : cancelled!", getId().getName());

    // if from external this handling can be cancelled
    // we need to properly wait for pending launched tasks
    // and after that -> call finish() and leave here!
    // todo

    finish();
}

//! Get the response headers (see: http://magpcss.org/ceforum/apidocs3/projects/(default)/CefResourceHandler.html)
void BackendResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
                                                int64& responseLength,
                                                CefString& redirectUrl)
{
    CEF_REQUIRE_IO_THREAD();

    response->SetMimeType("application/octet-stream");
    responseLength = c_debugResponse.size();  // quit the ReadResponse after 10bytes have been read (-1 for manual quitting)
}

//! Process the request (see: http://magpcss.org/ceforum/apidocs3/projects/(default)/CefResourceHandler.html)
bool BackendResourceHandler::ProcessRequest(CefRefPtr<CefRequest> request,
                                            CefRefPtr<CefCallback> cbResponseHeaderReady)
{
    CEF_REQUIRE_IO_THREAD();

    // Initialize the request
    /////////////////////////
    if(!initRequest(request))
    {
        // we dont handle this request or an error occured
        finish();
        return false;
    }

    // Read the response into a buffer (if existing)
    ////////////////////////////////////////////////
    std::optional<RequestCef::Payload> payload;
    if(!readPostData(request->GetPostData(), m_mimeType, payload, m_allocator))
    {
        finish();
        return false;
    }

    // Make a request and response wrapper which we hand
    // over to the message dispatcher.
    ////////////////////////////////////////////////////
    // Make a RequestCef (move the payload into it)
    RequestCef requestCef(m_requestId, std::move(payload));
    // Make a ResponseCef
    ResponseCef responseCef(cbResponseHeaderReady, m_allocator, false);
    // Get the future out
    m_responseFuture = ResponseFuture(responseCef);

    // Post a task to the UI-Thread to handle the message in the message dispatcher
    // which serializes the data, processes the request (its possible to launch a new worker thread, to not block UI-Thread),
    // and then serializes a proper reponse
    // if an error happens, the returned response contains no payload, but a well defined error message.
    // todo

    {  // DEBUG ==========
        m_bytesRead = 0;
        // FILE Threads does not block UI,
        CefPostTask(TID_FILE, base::Bind(&debugWaitOnOtherThread, cbResponseHeaderReady));

    }  // DEBUG ==========

    return true;
}

//! Read the response (see: http://magpcss.org/ceforum/apidocs3/projects/(default)/CefResourceHandler.html)
bool BackendResourceHandler::ReadResponse(void* dataOut,
                                          int bytesToRead,
                                          int& bytesRead,
                                          CefRefPtr<CefCallback> callback)
{
    CEF_REQUIRE_IO_THREAD();
    // Handle the repsponse
    // todo
    {  // DEBUG ==========
        if(m_bytesRead < c_debugResponse.size())
        {
            std::memcpy(dataOut, c_debugResponse.data() + m_bytesRead, 1);
            m_bytesRead++;
            bytesRead = 1;  // one byte read
            return true;
        }
        else
        {
            // Response is finished, we returned all bytes to read, so
            // finish up and return false.
            finish();
            return false;
        }
    }  // DEBUG ==========
}

//! Initilize the request by extracting the requestId and query string.
//! @return false if the request cannot be handled.
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

    // Extract MIME type
    CefRequest::HeaderMap headerMap;
    request->GetHeaderMap(headerMap);
    auto it = headerMap.find("Content-Type");
    if(it != headerMap.end())
    {
        m_mimeType = it->second;
        if(m_mimeType != "application/octet-stream" && m_mimeType != "application/json")
        {
            EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler: id: '{0}' : url '{1}': Content-Type: '{2}' can not be handled!",
                                      getId().getName(),
                                      url.ToString(),
                                      m_mimeType);
            return false;
        }
    }
    else
    {
        EXECGRAPHGUI_APPLOG_WARN("No 'Content-Type' header received -> post data will not be loaded!");
    }

    return true;
}

//! Finish handling the request: Reset everything and signal callback.
void BackendResourceHandler::finish()
{
    m_requestId.clear();
    m_query.clear();
    m_mimeType.clear();
}