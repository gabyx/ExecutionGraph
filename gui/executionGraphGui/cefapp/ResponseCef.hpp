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

#ifndef cefapp_ResponseCef_h
#define cefapp_ResponseCef_h

#include <cef_base.h>
#include <cef_request.h>
#include "common/Response.hpp"

class ResponsePromiseCef final : public ResponsePromise
{
public:
    using Payload = ResponsePromise::Payload;

public:
    template<typename... Args>
    ResponsePromiseCef(CefRefPtr<CefCallback> cbResponseHeaderReady, Args&&... args)
        : ResponsePromise(std::forward<Args>(args)...)
        , m_cbResponseHeaderReady(cbResponseHeaderReady)
    {}

    ~ResponsePromiseCef()
    {
        setResolveOnDestruction();
    }

private:
    virtual void setReadyImpl() override
    {
        // Signal that the response is available
        m_cbResponseHeaderReady->Continue();
        m_cbResponseHeaderReady = nullptr;  // Set to nullptr to mark that it has been called!
    }

    virtual void setCanceledImpl(const std::string& reason) override
    {
        // Signal that the response is available (an exception is set in the promise!)
        m_cbResponseHeaderReady->Continue();
        m_cbResponseHeaderReady = nullptr;  // Set to nullptr to mark that it has been called!
    }

private:
    CefRefPtr<CefCallback> m_cbResponseHeaderReady;  //!< The callback to call when the response header is ready.
};

using ResponseFutureCef = ResponseFuture;

#endif
