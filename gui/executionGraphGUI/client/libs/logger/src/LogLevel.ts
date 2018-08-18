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

export class LogLevel {
  constructor(private level: number, private levelStr: string) {
    this.level = level;
    this.levelStr = levelStr;
  }

  toString() {
    return this.levelStr;
  }

  isLessThanOrEqualTo(otherLevel: LogLevel) {
    return this.level <= otherLevel.level;
  }

  isGreaterThanOrEqualTo(otherLevel: LogLevel) {
    return this.level >= otherLevel.level;
  }

  isEqualTo(otherLevel: LogLevel) {
    return this.level === otherLevel.level;
  }
}

export const logLevels = {
  ALL: new LogLevel(Number.MIN_VALUE, 'ALL'),
  DEBUG: new LogLevel(10000, 'DEBUG'),
  INFO: new LogLevel(20000, 'INFO'),
  WARN: new LogLevel(30000, 'WARN'),
  ERROR: new LogLevel(40000, 'ERROR'),
  FATAL: new LogLevel(50000, 'FATAL'),
  OFF: new LogLevel(Number.MAX_VALUE, 'OFF')
};
