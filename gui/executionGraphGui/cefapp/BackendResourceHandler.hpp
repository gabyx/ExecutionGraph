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

#ifndef executionGraphGui_cefapp_BackendResourceHandler_h
#define executionGraphGui_cefapp_BackendResourceHandler_h

#include <cef_base.h>
#include <cef_resource_handler.h>
#include <functional>
#include <wrapper/cef_helpers.h>
#include <executionGraph/common/FileSystem.hpp>
#include <executionGraph/common/IObjectID.hpp>
#include "executionGraphGui/cefapp/ResponseCef.hpp"
class BackendRequestDispatcher;
class BufferPool;

/* ---------------------------------------------------------------------------------------*/
/*!
    Resource handler which handles binary data and dispatched to the backend.

    @date Sun Feb 18 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class BackendResourceHandler final : public CefResourceHandler,
                                     public executionGraph::IObjectID
{
    EXECGRAPH_NAMED_OBJECT_ID_DECLARATION

public:
    template<typename Deleter>
    BackendResourceHandler(std::shared_ptr<BackendRequestDispatcher> dispatcher,
                           std::shared_ptr<BufferPool> allocator,
                           Deleter&& deleter)
        : CefResourceHandler()
        , m_id("BackendResourceHandler")
        , m_dispatcher(dispatcher)
        , m_allocator(allocator)
        , m_deleter(deleter)
    {}

    virtual ~BackendResourceHandler()
    {
        CEF_REQUIRE_IO_THREAD();
    }

    //! CefResourceHandler overrides
    //@{
public:
    virtual void Cancel() override;

    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
                                    int64& responseLength,
                                    CefString& redirectUrl) override;

    virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
                                CefRefPtr<CefCallback> cbResponseHeaderReady) override;

    virtual bool ReadResponse(void* dataOut,
                              int bytesToRead,
                              int& bytesRead,
                              CefRefPtr<CefCallback> callback) override;
    //@}

private:
    std::path m_requestTarget;  //!< The request type "<category/subcategory>" (e.g. "graphManip/addNode" ).
    std::string m_query;     //!< The additional query (everything after "?").
    std::string m_mimeType;  //!< MIME type of the post data.
    bool initRequest(CefRefPtr<CefRequest> request);

private:
    void reset();

private:
    std::shared_ptr<BackendRequestDispatcher> m_dispatcher;  //!< The dispatcher to which request/response get dispatched.
    std::shared_ptr<BufferPool> m_allocator;                 //!< The allocator for `BinaryPayload` of request and responses.
    ResponseFutureCef m_responseFuture;                      //!< The response future we await in the resource handler.
    ResponseFutureCef::Payload m_payload;                    //!< The moved payload from the future. (default = empty)

    uint8_t* m_buffer        = nullptr;
    std::size_t m_bufferSize = 0;
    std::size_t m_bytesRead  = 0;  //!< Number of bytes read.

    //! CefRefCounted overrides
    //@{
public:
    void AddRef() const override { m_refCount.AddRef(); }
    bool Release() const override;
    bool HasOneRef() const override { return m_refCount.HasOneRef(); }

private:
    const std::function<void(BackendResourceHandler*)> m_deleter;  //!< Special deleter since we allocated over a pool!
    CefRefCount m_refCount;
    //@}
};

#endif