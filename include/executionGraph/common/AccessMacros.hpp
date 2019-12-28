// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Fri Feb 23 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#define EG_DISALLOW_COPY(Class) \
public:                                \
    Class(const Class&) = delete;      \
    Class& operator=(const Class&) = delete;

#define EG_DISALLOW_MOVE(Class) \
public:                                \
    Class(Class&&) = delete;           \
    Class& operator=(Class&&) = delete;

#define EG_DISALLOW_COPY_AND_MOVE(Class) \
    EG_DISALLOW_COPY(Class)              \
    EG_DISALLOW_MOVE(Class)
