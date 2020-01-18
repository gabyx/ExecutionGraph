//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Tue Jan 09 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#pragma once

#include <rttr/type>
#include "executionGraph/common/Identifier.hpp"

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
        Interface which provides an ID.

        @date Sun Feb 18 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    class IObjectID
    {
        RTTR_ENABLE()
    protected:
        IObjectID() = default;

    public:
        virtual ~IObjectID() = default;

    public:
        virtual const Id& id() const = 0;
    };
}  // namespace executionGraph

// clang-format off
#define EG_OBJECT_ID_DECLARATION_IMPL(TMemberId, OverrideKeyWord) \
public:                                                                  \
    using Id = executionGraph::Id;                                       \
    const Id& id() const OverrideKeyWord { return m_id; }       \
                                                                         \
private:                                                                 \
    const executionGraph::TMemberId m_id;

#define EG_OBJECT_ID_DECLARATION \
    EG_OBJECT_ID_DECLARATION_IMPL(Id, override final)
#define EG_OBJECT_ID_NON_VIRTUAL_DECLARATION \
    EG_OBJECT_ID_DECLARATION_IMPL(Id, )

#define EG_NAMED_OBJECT_ID_DECLARATION_IMPL(OverrideKeyWord)              \
    EG_OBJECT_ID_DECLARATION_IMPL(IdNamed, OverrideKeyWord)               \
public:                                                                          \
    using IdNamed = executionGraph::IdNamed;                                     \
    std::string getName() const { return m_id.getName(); }                       \
    const std::string& getShortName() const { return m_id.getShortName(); }

#define EG_NAMED_OBJECT_ID_DECLARATION \
    EG_NAMED_OBJECT_ID_DECLARATION_IMPL(override)
#define EG_NAMED_OBJECT_ID_NON_VIRTUAL_DECLARATION \
    EG_NAMED_OBJECT_ID_DECLARATION_IMPL()

// clang-format on
