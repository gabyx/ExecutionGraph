// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Thu Feb 22 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef cefapp_IRequest_h
#define cefapp_IRequest_h

#include <rttr/type>
#include <string>
#include "cefapp/IResponse.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    General Request Message

    @date Thu Feb 22 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class IRequest
{
    RTTR_ENABLE()

protected:
    IRequest() = default;

public:
    virtual ~IRequest() = default;

public:
    //! Get the request id describing this message.
    virtual std::string getRequestId() = 0;

public:
    //! Get the response object.
    virtual IResponse& getResponse() = 0;

public:
    //! Callback for signaling that the request is fullfilled and the response object is available.
    virtual void ready() = 0;

    //! Callback for signaling that this request is cancled.
    virtual void cancel() = 0;
};

#endif