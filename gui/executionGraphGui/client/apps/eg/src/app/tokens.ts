// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Thu Aug 16 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { InjectionToken } from '@angular/core';

export const VERBOSE_LOG_TOKEN = new InjectionToken<boolean>('VERBOSE_LOG_TOKEN');
export const BINARY_HTTP_ROUTER_BASE_URL = new InjectionToken<string>('BINARY_HTTP_ROUTER_BASE_URL');
