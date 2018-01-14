//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//! 
//!  @date Sun Jan 14 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//! 
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraphGui_backend_ExecutionGraphBackend_hpp
#define executionGraphGui_backend_ExecutionGraphBackend_hpp

#include "backend/IBackend.hpp"

class ExecutionGraphBackend : public IBackend,                         
{
    public:
        using IBackend::Identifier;
        static const Identifier defaultIdentifier;

    public:
        ExecutionGraphBackend(const Identifier& id = defaultIdentifier) : m_id(id) = default;
        virtual ~ExecutionGraphBackend() = default;

    //! IIdentifier Interface
    //@{
    public:
        const Identifier& getID() {return m_id;};
    private:
        const Identifier m_id;
    //@}
};
};

#endif