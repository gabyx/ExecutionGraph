// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Mon Mar 05 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGui_cefapp_ResponseCef_h
#define executionGraphGui_cefapp_ResponseCef_h

#include <cef_base.h>
#include <cef_request.h>
#include "executionGraphGui/common/Response.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    A response for which implements the beaviour for CEF in the `BackendResourceHandler`.

    @date Sun Mar 11 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class ResponsePromiseCef final : public ResponsePromise
{
public:
    using Payload = ResponsePromise::Payload;
    using Base    = ResponsePromise;

public:
    template<typename... Args>
    ResponsePromiseCef(CefRefPtr<CefCallback> cbResponseHeaderReady,
                       Args&&... args)
        : ResponsePromise(std::forward<Args>(args)...)
        , m_cbResponseHeaderReady(cbResponseHeaderReady)
    {}

    ~ResponsePromiseCef()
    {
        setResolveOnDestruction();
    }

    ResponsePromiseCef(ResponsePromiseCef&&) = default;
    ResponsePromiseCef& operator=(ResponsePromiseCef&&) = default;

private:
    void setReady(Payload&& payload) override
    {
        Base::setReady(std::move(payload));
        // Signal that the response is available
        m_cbResponseHeaderReady->Continue();
        m_cbResponseHeaderReady = nullptr;  // Set to nullptr to mark that it has been called!
    }

    void setCanceled(std::exception_ptr exception) override
    {
        Base::setCanceled(exception);
        // Signal that the response is available (an exception is set in the promise!)
        m_cbResponseHeaderReady->Continue();
        m_cbResponseHeaderReady = nullptr;  // Set to nullptr to mark that it has been called!
    }

private:
    CefRefPtr<CefCallback> m_cbResponseHeaderReady;  //!< The callback to call when the response header is ready.
};

//! The response future corresponding to `ResponsePromiseCef`.
using ResponseFutureCef = ResponseFuture;

#endif
