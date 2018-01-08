#!/bin/bash

change="// ========================================================================================
\n//  ExecutionGraph
\n//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
\n// 
\n// @date Mon Jan 08 2018
\n// @author Gabriel Nützi, <gnuetzi (at) gmail (døt) com>
\n// 
\n//  This Source Code Form is subject to the terms of the Mozilla Public
\n//  License, v. 2.0. If a copy of the MPL was not distributed with this
\n//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
\n// ========================================================================================"

sedCmd="s@\/\/ =+\n\/\/  ExecutionGraph.*\/\/ =+@${change}@gs"

find . -type f -name ".cpp" -exec sed -i "" sedCmd