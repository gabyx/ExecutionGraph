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
#include "cefapp/Response.hpp"

class ResponseCef final : public Response
{
public:
    using Payload = Response::Payload;

public:
    ResponseCef(CefRefPtr<CefResponse> response, CefRefPtr<CefCallback> responseHeaderReady)
        : m_response(response), m_responseHeaderReady(responseHeaderReady)
    {}

private:
    virtual void setReady() override
    {
        // Signal that the response is available
        m_responseHeaderReady.Continue();
    }

    virtual void setCanceled(const std::string& reason) override
    {
        //todo implement setCancled
        // Set the status code to failed
        m_response.SetError(cef_errorcode_t::ERR_FAILED);

        // Serialize a default standardized error message payload
        // todo

        // Signal that the response is available
        m_responseHeaderReady.Continue();
    }

private:
    CefRefPtr<CefResponse> m_response;             //!< The CEF Response.
    CefRefPtr<CefCallback> m_responseHeaderReady;  //!< The callback to call when the response header is ready.
};

#endif
