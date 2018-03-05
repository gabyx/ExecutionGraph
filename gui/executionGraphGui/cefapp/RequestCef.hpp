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

#ifndef cefapp_RequestCef_h
#define cefapp_RequestCef_h

#include <cef_base.h>
#include <cef_request.h>
#include <optional>
#include "cefapp/Request.hpp"

class RequestCef final : public Request
{
public:
    using Payload = Request::Payload;

public:
    RequestCef(const std::string& requestId,
               CefRefPtr<CefRequest> request,
               const std::optional<Payload>& payload)
        : Request(requestId), m_payload(payload), m_request(request)
    {}

    virtual ~RequestCef() = default;

    virtual const Payload* getPayload() const override
    {
        return m_payload ? &(*m_payload) : nullptr;
    };

private:
    std::optional<Payload> payload;  //!< The payload of the message;
    CefRefPtr<CefRequest> m_request  //!< The CEF request.
};

#endif