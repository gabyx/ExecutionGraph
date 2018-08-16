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

import { LoggerFactory } from "./ILoggerFactory";
import { MultiSinkLogger } from "./MultiSinkLogger";
import { ConsoleSink } from "./ConsoleSink";
import { SimpleFormatter } from "./SimpleFormatter";
import { ILogger } from "./ILogger";
/**
 * A simple factory which creates a console logger.
 */
export class SimpleConsoleLoggerFactory extends LoggerFactory {
  create(name: string) : ILogger {
    let formatter = new SimpleFormatter();
    return new MultiSinkLogger(name,
      [
        new ConsoleSink(formatter)
        // @todo: here could we introduce another appender which sends to the backend (other factory of course)
      ]);
  }
}
