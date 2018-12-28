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

#include "executionGraphGui/cefapp/BackendResourceHandler.hpp"
#include <algorithm>
#include <array>
#include <cef_parser.h>
#include <chrono>
#include <thread>
#include <wrapper/cef_closure_task.h>
#include <wrapper/cef_helpers.h>
#include <executionGraph/common/Assert.hpp>
#include "executionGraphGui/cefapp/BackendRequestDispatcher.hpp"
#include "executionGraphGui/cefapp/RequestCef.hpp"
#include "executionGraphGui/cefapp/ResponseCef.hpp"
#include "executionGraphGui/common/BinaryBuffer.hpp"
#include "executionGraphGui/common/Exception.hpp"
#include "executionGraphGui/common/Loggers.hpp"
#include "executionGraphGui/common/RequestError.hpp"

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
        EXECGRAPHGUI_ASSERT(!mimeType.empty(), "No MIME-type!");

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
            BinaryBuffer<BufferPool> buffer(allocator, element->GetBytesCount());
            element->GetBytes(buffer.size(), static_cast<void*>(buffer.data()));
            EXECGRAPHGUI_LOGCODE_DEBUG(printPostData(buffer));
            EXECGRAPHGUI_APPLOG_DEBUG("BackendResourceHandler: Read last post data element: bytes: '{0}'.", element->GetBytesCount());
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
    EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler: '{0}' : cancelled!", getName());

    // if from external this handling can be cancelled
    // we need to properly wait for pending launched tasks
    // and after that -> call finish() and leave here!
    // todo

    reset();
}

//! Get the response headers (see: http://magpcss.org/ceforum/apidocs3/projects/(default)/CefResourceHandler.html)
void BackendResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
                                                int64& responseLength,
                                                CefString& redirectUrl)
{
    CEF_REQUIRE_IO_THREAD();

    std::string error = "Unknown Exception";

    responseLength = 0;
    m_bytesRead    = 0;

    try
    {
        EXECGRAPHGUI_THROW_IF(!m_responseFuture.isValid(), "Future is invalid!");

        m_payload = m_responseFuture.waitForPayload();  // Get the payload!

        m_bufferSize = responseLength = m_payload.buffer().size();  // set the response byte size (can be empty)
        if(responseLength)
        {
            response->SetMimeType(m_payload.mimeType());  // set the mime type
            m_buffer = m_payload.buffer().data();      // set the buffer pointer (can be nullptr!)
        }
        response->SetStatusText("handled");
        response->SetStatus(200);  // http status code: 200 := The request has been handled! (https://developer.mozilla.org/en-US/docs/Web/HTTP/Status)
        return;
    }
    catch(const BadRequestError& e)
    {
        EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler: Bad request: '{0}'", e.what());
        response->SetStatusText(e.what());
        response->SetStatus(400);  // http status code: 400 : Bad request!
        return;
    }
    // Every other Exception is an internal server error!
    // and is fatal!
    catch(const InternalBackendError& e)
    {
        error = e.what();
    }
    catch(std::exception& e)
    {
        error = e.what();
    }

    EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler: Exception in GetResponseHeaders");
    response->SetStatusText(error);
    response->SetStatus(500);  // http status code: 500 : Internal server error!
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
        return false;
    }

    // Read the response into a buffer (if existing)
    ////////////////////////////////////////////////
    std::optional<RequestCef::Payload> payload;
    if(!m_mimeType.empty() && !readPostData(request->GetPostData(), m_mimeType, payload, m_allocator))
    {
        return false;
    }

    // Make a request and response wrapper which we hand
    // over to the message dispatcher.
    ////////////////////////////////////////////////////
    // Make a RequestCef (move the payload into it)
    auto requestCef = std::make_unique<RequestCef>(m_requestTarget, std::move(payload));
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

    EXECGRAPHGUI_ASSERT(m_buffer, "We need a buffer set! We should not come to this method!");

    // Handle the repsponse
    if(m_bytesRead < m_bufferSize)
    {
        // Copy as many bytes as possible into the output buffer
        std::size_t nBytes = std::min(std::size_t(bytesToRead), m_bufferSize - m_bytesRead);
        std::memcpy(dataOut, m_buffer + m_bytesRead, nBytes);
        m_bytesRead += nBytes;
        bytesRead = nBytes;
        return true;
    }
    else
    {
        bytesRead = 0;
        return false;
    }
}

//! Initilize the request by extracting the requestId and query string.
//! @return false if the request cannot be handled.
bool BackendResourceHandler::initRequest(CefRefPtr<CefRequest> request)
{
    reset();

    CefString url = request->GetURL();
    CefURLParts urlParts;
    if(!CefParseURL(url, urlParts))
    {
        EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler: '{0}' : url '{1}': url parse failed!",
                                  getName(),
                                  url.ToString());
        return false;
    }

    // Exctract requestId
    // e.g. m_requestTarget := "catergory/subcategory/command"
    m_requestTarget = executionGraph::splitLeadingSlashes(CefString(urlParts.path.str).ToString());

    if(m_requestTarget.empty())
    {
        EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler '{0}' : url '{1}': requestId extract failed!",
                                  getName(),
                                  url.ToString());
        return false;
    }

    // Extract additional query
    m_query = CefString(urlParts.query.str).ToString();

    // Extract MIME type
    CefRequest::HeaderMap headerMap;
    request->GetHeaderMap(headerMap);
    m_mimeType.clear();
    auto it = headerMap.find("Content-Type");
    if(it != headerMap.end())
    {
        m_mimeType = it->second;
        if(m_mimeType != "application/octet-stream" && m_mimeType != "application/json")
        {
            EXECGRAPHGUI_APPLOG_ERROR("BackendResourceHandler: '{0}' : url '{1}': Content-Type: '{2}' can not be handled!",
                                      getName(),
                                      url.ToString(),
                                      m_mimeType);
            return false;
        }
    }

    // No 'Content-Type' header received -> post data will not be loaded!
    return true;
}

//! Finish handling the request: Reset everything and signal callback.
void BackendResourceHandler::reset()
{
    m_requestTarget.clear();
    m_query.clear();
    m_mimeType.clear();
    m_buffer     = nullptr;
    m_bufferSize = 0;
    m_bytesRead  = 0;
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