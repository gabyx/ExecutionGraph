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

import { LogLevel } from './LogLevel';

export class LogEvent {
  constructor(public _level: LogLevel, public _data: any[]) {}

  public get level() {
    return this._level;
  }
  public get data() {
    return this._data;
  }
}
