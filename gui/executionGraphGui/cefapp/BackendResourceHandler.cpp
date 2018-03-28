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
#include <algorithm>
#include <array>
#include <cef_parser.h>
#include <chrono>
#include <executionGraph/common/Assert.hpp>
#include <executionGraph/common/Exception.hpp>
#include <thread>
#include <wrapper/cef_closure_task.h>
#include <wrapper/cef_helpers.h>
#include "cefapp/BackendRequestDispatcher.hpp"
#include "cefapp/RequestCef.hpp"
#include "cefapp/ResponseCef.hpp"
#include "common/BinaryBuffer.hpp"
#include "common/Loggers.hpp"

namespace
{
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
            EXECGRAPHGUI_LOGCODE_DEBUG(printPostData(buffer));
            EXECGRAPHGUI_APPLOG_DEBUG("BackendResourceHandler: Read last post data element: bytes: {0}.", element->GetBytesCount());
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

    auto& future = m_responseFuture.getFuture();

    try
    {
        EXECGRAPH_THROW_EXCEPTION_IF(!future.valid(), "Future is invalid!");

        m_payload = future.get();  // Set the payload!

        response->SetMimeType(m_payload.getMIMEType());    // set the mime type
        responseLength = m_payload.getBuffer().getSize();  // set the response byte size
        response->SetStatusText("success");
        response->SetStatus(200);  // http status code: 200 := The request has succeeded! (https://developer.mozilla.org/en-US/docs/Web/HTTP/Status)

        m_bytesRead = 0;
    }
    catch(const std::exception& e)
    {
        // Exception while processing the request -> abort!
        EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler: Exception in GetResponseHeaders:'{0}", e.what());
        response->SetStatusText(e.what());
        response->SetStatus(400);  // http status code: 400 : Bad request!
        response->SetError(cef_errorcode_t::ERR_FAILED);
    }
    catch(...)
    {
        // Exception while processing the request -> abort!
        EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler: Unknown exception in GetResponseHeaders");
        response->SetStatusText("Unknown Exception!");
        response->SetStatus(400);  // http status code: 400 : Bad request!
        response->SetError(cef_errorcode_t::ERR_FAILED);
    }
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
    auto requestCef = std::make_unique<RequestCef>(m_requestURL, std::move(payload));
    // Make a ResponseCef
    auto responseCef = std::make_unique<ResponsePromiseCef>(cbResponseHeaderReady, requestCef->getId(), m_allocator, true);
    // Get the future out
    m_responseFuture = ResponseFuture(*responseCef);

    // Add the request to the dispatcher (multi-threaded)
    m_dispatcher->handleRequest(std::move(requestCef), std::move(responseCef));

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

    auto bufferSize = m_payload.getBuffer().getSize();

    if(m_bytesRead < bufferSize)
    {
        // Copy as many bytes as possible into the output buffer
        std::size_t nBytes = std::min(std::size_t(bytesToRead), bufferSize - m_bytesRead);
        std::memcpy(dataOut, m_payload.getBuffer().getData() + m_bytesRead, nBytes);
        m_bytesRead += nBytes;
        bytesRead = nBytes;
        return true;
    }
    else
    {
        // Response is finished, we copied all bytes into the output buffer, so
        // finish up and return false.
        finish();
        return false;
    }
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
    // e.g. m_requestURL := "catergory/subcategory/command"
    m_requestURL = executionGraph::splitLeadingSlashes(CefString(urlParts.path.str).ToString());

    if(m_requestURL.empty())
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
    m_requestURL.clear();
    m_query.clear();
    m_mimeType.clear();
}

//! Release method for CefRefCounted
bool BackendResourceHandler::Release() const
{
    if(m_refCount.Release())
    {
        CEF_REQUIRE_IO_THREAD();
        m_deleter(const_cast<BackendResourceHandler*>(this));

        return true;
    }
    return false;
}