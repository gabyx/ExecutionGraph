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

#ifndef executionGraphGui_common_Request_hpp
#define executionGraphGui_common_Request_hpp

#include <optional>
#include <string>
#include <rttr/type>
#include <executionGraph/common/FileSystem.hpp>
#include <executionGraph/common/IObjectID.hpp>
#include "executionGraphGui/common/BinaryPayload.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    General Request Message

    A `BackendRequestHandler` is handling such a request. It can be registered in
    the message dispatcher for handling one or several request targets 
    (see `m_requestTarget`).
    The request target is a normal absolute path.

    The serializer which decodes the payload is kept outside of this class 
    and other derived ones.

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

public:
    Request(std::path requestTarget,
            std::optional<Payload> payload = std::nullopt)
        : m_id()
        , m_requestTarget(std::move(requestTarget))
        , m_payload{std::move(payload)}
    {}

    virtual ~Request() = default;

    Request(Request&&) = default;
    Request& operator=(Request&&) = default;

    Request(const Request&) = delete;
    Request& operator=(const Request&&) = delete;

public:
    //! Get the request target describing this message.
    const std::path& getTarget() const { return m_requestTarget; }

public:
    //! Get the payload of this request.
    const std::optional<Payload>& getPayload() const { return m_payload; }
    //! Get the payload of this request.
    std::optional<Payload>& getPayload() { return m_payload; }

private:
    //! The request target path
    //! e.g. "/executiongraph-backend/graph/addNode", 
    //! e.g. "/executiongraph-backend/general/addGraph" 
    //! etc.
    std::path m_requestTarget;

    //! The optional payload.
    std::optional<Payload> m_payload;
};

#endif
