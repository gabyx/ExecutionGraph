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

#ifndef cefapp_BackendResourceHandler_h
#define cefapp_BackendResourceHandler_h

#include <cef_base.h>
#include <cef_resource_handler.h>
#include <executionGraph/common/FileSystem.hpp>
#include <executionGraph/common/IObjectID.hpp>
#include <functional>
#include <wrapper/cef_helpers.h>
#include "cefapp/Response.hpp"
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
    EXECGRAPH_OBJECT_ID_DECLARATION

public:
    template<typename Deleter>
    BackendResourceHandler(std::shared_ptr<BufferPool> allocator, Deleter&& deleter)
        : CefResourceHandler()
        , m_id("BackendResourceHandler")
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
    std::path m_requestId;   //!< The request id "<category/subcategory>" (e.g. "graphManip/addNode" ).
    std::string m_query;     //!< The additional query (everything after "?").
    std::string m_mimeType;  //!< MIME type of the post data.
    bool initRequest(CefRefPtr<CefRequest> request);

private:
    void finish();

private:
    std::shared_ptr<BufferPool> m_allocator;

    std::size_t m_bytesRead = 0;  // DEBUG ==========
    ResponseFuture m_responseFuture;

    //! CefRefCounted overrides
    //@{
public:
    void AddRef() const override { m_refCount.AddRef(); }
    bool Release() const override
    {
        if(m_refCount.Release())
        {
            CEF_REQUIRE_IO_THREAD();
            m_deleter(const_cast<BackendResourceHandler*>(this));

            return true;
        }
        return false;
    }
    bool HasOneRef() const override { return m_refCount.HasOneRef(); }

private:
    const std::function<void(BackendResourceHandler*)> m_deleter;  //!< Special deleter since we allocated over a pool!
    CefRefCount m_refCount;
    //@}
};

#endif