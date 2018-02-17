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
#include "cefapp/Loggers.hpp"

namespace
{
    const std::array<uint8_t, 10> c_debugResponse = {'e', 'x', 'e', 'c', 'g', 'r', 'a', 'p', 'h', '!'};

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

    //! Wait some time in another other to simulate working.
    void debugWaitOnOtherThread(CefRefPtr<CefCallback> callback)
    {
        using namespace std::chrono_literals;

        EXECGRAPHGUI_APPLOG_DEBUG("Computing started [2sec] ...");
        std::this_thread::sleep_for(1.3s);
        EXECGRAPHGUI_APPLOG_DEBUG("Computing finished!");

        callback->Continue();  // Signal that response headers are here.
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
    responseLength = c_debugResponse.size();  // quit the ReadResponse after 10bytes have been read (-1 for manual quitting)
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

    // Setup thread-safe binary buffer for post data and response data.
    // todo

    // Post a task to the UI-Thread to handle the message in the message dispatcher
    // which serializes the data, processes the request (its possible to launch a new worker thread, to not block UI-Thread),
    // and then serializes a proper reponse
    // if an error happens, the returned response contains no payload, but a well defined error message.
    // todo

    {  // DEBUG ==========
        m_bytesRead = 0;
        // FILE Threads does not block UI,
        CefPostTask(TID_FILE, base::Bind(&debugWaitOnOtherThread, callback));
    }  // DEBUG ==========

    return true;
}
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
        }

        if(m_bytesRead == c_debugResponse.size())
        {
            // Response is finished, we returned all bytes to read, so
            // finish up and return false.
            finish();
            return false;
        }

        return true;
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
    m_bytesRead = 0;
}
