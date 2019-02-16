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

#pragma once

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
    (see `m_target`).
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
            std::string requestTargetArgs,
            std::optional<Payload> payload = std::nullopt)
        : m_id()
        , m_target(std::move(requestTarget))
        , m_targetArgs(std::move(requestTargetArgs))
        , m_payload{std::move(payload)}
    {}

    virtual ~Request() = default;

    Request(Request&&) = default;
    Request& operator=(Request&&) = default;

    Request(const Request&) = delete;
    Request& operator=(const Request&&) = delete;

public:
    //! Get the request target describing this message.
    const std::path& target() const { return m_target; }
    //! Get the request target additional arguments.
    const std::string& targetArgs() const { return m_targetArgs; }

public:
    //! Get the payload of this request.
    const std::optional<Payload>& payload() const { return m_payload; }
    //! Get the payload of this request.
    std::optional<Payload>& payload() { return m_payload; }

private:
    //! The request target path in the form
    //! "/eg-backend/<categeory>/<subcategory>":
    //! e.g. "/eg-backend/graph/addNode",
    //! e.g. "/eg-backend/general/addGraph"
    //! e.g. "/eg-backend/general/files" etc.
    std::path m_target;

    //! Additional request arguments (if any, normally empty).
    std::string m_targetArgs;

    //! The optional payload.
    std::optional<Payload> m_payload;
};
