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

#ifndef cefapp_Request_h
#define cefapp_Request_h

#include <executionGraph/common/IObjectID.hpp>
#include <rttr/type>
#include <string>
#include "common/BinaryPayload.hpp"
#include "common/Response.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    General Request Message

    A BackendRequestHandler is handling such a request. It can be registered in
    the message dispatcher for handling one or several request ids (see `m_requestType`)
    The request id is in the form "category/subcategory" (e.g. "graphManip/addNode").
    We use a category and a subcategory to be able to structure requests into groups.
    Also for the future, when more and more requests get added.

    The serializer which decodes the payload is kept outside of this class and other derived
    ones.

    @date Thu Feb 22 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class Request : public executionGraph::IObjectID
{
    EXECGRAPH_OBJECT_ID_DECLARATION
    RTTR_ENABLE()

public:
    using Payload = BinaryPayload;

protected:
    Request(const std::string& requestType)
        : m_id("Request-" + requestType)
        , m_requestType(requestType)
    {}

public:
    virtual ~Request() = default;

public:
    //! Get the request id describing this message.
    virtual const std::string& getRequestType() { return m_requestType; }

public:
    //! Get the payload of this request. Nullptr if there is no payload for this request.
    //! The return value does not need to be thread-safe.
    virtual const Payload* getPayload() const = 0;

private:
    //! The requestType "<category>/<subcategory>" (e.g. "graphManip/addNode")
    //! A BackendRequestHandler handling such a requestType can be registered on
    //! a set of such requestIds.
    std::string m_requestType;
};

#endif
