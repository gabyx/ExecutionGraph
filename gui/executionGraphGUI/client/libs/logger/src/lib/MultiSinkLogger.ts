// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Aug 15 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { ILogger } from './ILogger';
import { ISink } from './ISink';
import { LogLevel, logLevels } from './LogLevel';
import { LogEvent } from './LogEvent';

function prepend(value, array) {
  var newArray = array.slice();
  newArray.unshift(value);
  return newArray;
}

/**
 * Multi-sink logger implementing the ILogger interface.
 */
export class MultiSinkLogger implements ILogger {
  constructor(private name: string, private sinks: ISink[]) {}

  public debug(...data: any[]) {
    this.write(logLevels.DEBUG, data);
  }
  public info(...data: any[]) {
    this.write(logLevels.INFO, data);
  }
  public warn(...data: any[]) {
    this.write(logLevels.WARN, data);
  }
  public error(...data: any[]) {
    this.write(logLevels.ERROR, data);
  }

  private write(level: LogLevel, data: any[]) {
    let logEvent = new LogEvent(level, prepend(`[${this.name}]`, data));
    for (let sink of this.sinks) {
      sink.write(logEvent);
    }
  }
}
