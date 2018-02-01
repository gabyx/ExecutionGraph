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

#include <rttr/type>
#include "backend/Backend.hpp"

class ExecutionGraphBackend : public Backend
{
    RTTR_ENABLE()
public:
    using Backend::Id;

public:
    ExecutionGraphBackend()
        : Backend("ExecutionGraphBackend")
    {
    }
    virtual ~ExecutionGraphBackend() override = default;
};

#endif
