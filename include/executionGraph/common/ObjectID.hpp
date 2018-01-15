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

#ifndef executionGraph_common_ObjectID_hpp
#define executionGraph_common_ObjectID_hpp

#include "executionGraph/common/Identifier.hpp"

namespace executionGraph{

    //! Interface which provides an ID.
    class IObjectID
    {
    public:
        using Id = executionGraph::Id;
    protected:
        virtual ~IObjectID() = default;

    public:
        virtual const Id& getId() const = 0;
    };

    //! Class which provides an ID.
    class ObjectID : public IObjectID
    {
    public:
        using Id = executionGraph::Id; 
    protected:
        ObjectID(const Id& id) : m_id(id){}
        virtual ~ObjectID() override = default;

    public:
        const Id& getId() const override final { return m_id; }
    private:
        const Id m_id;
    };

}

#endif