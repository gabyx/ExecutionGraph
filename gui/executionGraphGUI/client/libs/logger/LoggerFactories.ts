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

import { ILoggerFactory } from "@eg/logger/ILoggerFactory";
import { MultiSinkLogger } from "@eg/logger/MultiSinkLogger";
import "./ConsoleAppender"

/**
 * A simple factory which creates a console logger.
 */
export class SimpleConsoleLoggerFactory implements ILoggerFactory {
  create(name: string) {
    return new MultiSinkLogger(name,
      [
        {
          type: "console", level: ["debug", "info", "trace"]
        }
        // @todo: here could we introduce another appender which sends to the backend (other factory of course)
      ]);
  }
}
