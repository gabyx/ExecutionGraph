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

#ifndef common_Request_hpp
#define common_Request_hpp

#include <executionGraph/common/FileSystem.hpp>
#include <executionGraph/common/IObjectID.hpp>
#include <rttr/type>
#include <string>
class BinaryPayload;

/* ---------------------------------------------------------------------------------------*/
/*!
    General Request Message

    A BackendRequestHandler is handling such a request. It can be registered in
    the message dispatcher for handling one or several request ids (see `m_requestURL`)
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
    Request(const std::path& requestURL)
        : m_id("Request-" + requestURL.string())
        , m_requestURL(requestURL)
    {}

public:
    virtual ~Request() = default;

public:
    //! Get the request url describing this message.
    const std::path& getURL() const { return m_requestURL; }
    //! Set the request url describing this message.
    void setURL(const std::path& requestURL) { m_requestURL = requestURL; }

public:
    //! Get the payload of this request. Nullptr if there is no payload for this request.
    //! The return value does not need to be thread-safe.
    virtual const Payload* getPayload() const = 0;

private:
    //! The request URL (it will get adjusted during request forwarding)
    //! e.g. "graph/6fdb1cb9-2d2b-46ba-93b5-0b7083ea28c3/addNode"
    //! e.g. "general/addGraph"
    std::path m_requestURL;
};

#endif
