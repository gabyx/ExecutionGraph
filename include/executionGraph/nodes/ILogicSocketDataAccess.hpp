// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Tue Apr 07 2020
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include "executionGraph/nodes/LogicDataHandle.hpp"

namespace executionGraph
{
    //! Base class
    class ILogicSocketDataAccessBase
    {
    public:
        virtual ~ILogicSocketDataAccessBase() = default;
    };

    /* ---------------------------------------------------------------------------------------*/
    /*!
        Interface providing data access functionality for a socket data.

        @date Tue Apr 07 2020
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TData>
    class ILogicSocketDataAccess : public ILogicSocketDataAccessBase
    {
    public:
        using Data            = TData;
        using DataHandle      = LogicDataHandle<Data>;
        using DataHandleConst = LogicDataHandle<const Data>;

    public:
        virtual ~ILogicSocketDataAccess() = default;

        virtual DataHandleConst dataHandleConst() const noexcept = 0;
        virtual DataHandleConst dataHandle() const noexcept      = 0;
        virtual DataHandle dataHandle() noexcept                 = 0;
    };

}  // namespace executionGraph