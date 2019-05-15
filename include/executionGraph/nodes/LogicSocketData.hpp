// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed May 15 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

//! Data wrapper for the output socket.
template<typename TData>
class LogicSocketData
{
public:
    using DataType = TData;

public:
    template<typename T>
    LogicSocketData(T&& value)
        : m_data(std::forward<T>(value)) {}

    inline DataType& data() { return m_data; }
    inline const DataType& data() const { return m_data; }

private:
    DataType m_data;  //!< The data.
};