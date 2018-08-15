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

import { Appender, BaseAppender, LogEvent, Logger } from "ts-log-debug";
import { ILogger } from "@eg/logger/ILogger";

/**
 * Multi-sink logger implementing the ILogger interface.
 */
export class MultiSinkLogger implements ILogger {

  private logger: Logger;

  constructor(name: string, appenderConfigs: any[], log?: Logger) {
    if (!log) {
      this.logger = new Logger(name);
    }
    else {
      this.logger = log;
    }

    // Adding the appenders to the logger
    for (let i = 0; i < appenderConfigs.length; ++i) {
      this.logger.appenders.set(`appender-${i}`,
        appenderConfigs[i]
      );
    }
  }

  logDebug(message: string) {
    this.logger.debug(message);
  }
  logInfo(message: string) {
    this.logger.info(message);
  }
  logWarn(message: string) {
    this.logger.warn(message);
  }
  logError(message: string) {
    this.logger.error(message);
  }
}


