// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Mon Mar 05 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef cefapp_BinaryBufferView_hpp
#define cefapp_BinaryBufferView_hpp

#include <cstdint>

/* ---------------------------------------------------------------------------------------*/
/*!
    Simple Binary Buffer View

    @date Mon Mar 05 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class BinaryBufferView
{
public:
    BinaryBuffer(uint8_t* const data, std
                 : size_t bytes)
        : m_data(data), m_bytes(bytes)
    {}

    uint8_t* getData() { return m_data; }
    std::size_t getBytes() { return m_bytes; }

private:
    uint8_t* const m_data = nullptr;  //!< Data pointer.
    std::size_t m_bytes   = 0;        //!< Number of bytes.
};

#endif