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
class BinaryBuffer;

/* ---------------------------------------------------------------------------------------*/
/*!
    General Request Message

    A BackendMessageHandler is handling such a request. It can be registered in
    the message dispatcher for handling one or several request ids (see `m_requestId`)
    The request id is in the form "category/subcategory" (e.g. "graphManip/addNode").
    We use a category and a subcategory to be able to structure requests into groups.
    Also for the future, when more and more requests get added.

    The serializer which decodes the payload is kept outside of this class and other derived
    ones.

    @date Thu Feb 22 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class Request
{
    RTTR_ENABLE()

protected:
    Request(const std::string& requestId)
        : m_requestId(requestId) {}

public:
    virtual ~Request() = default;

public:
    //! Get the request id describing this message.
    virtual std::string getRequestId() = 0;

public:
    //! Get the payload of this request. Nullptr if there is no payload for this request.
    //! The return value does not need to be thread-safe.
    virtual const BinaryBuffer* getPayload() = 0;

    //! Get the MimeType of the payload.
    //! Could be "application/octet-stream" for a binary file or
    //! "application/json" for a json file in utf-8 encoding.
    virtual const std::string& getMimeType() = 0;

private:
    //! The requestId (e.g. "category/subcategory" -> "graphManip/addNode")
    //! A BackendMessageHandler handling such a request can be registered on
    //! a set of such requestIds.
    std::string m_requestId;
};

#endif