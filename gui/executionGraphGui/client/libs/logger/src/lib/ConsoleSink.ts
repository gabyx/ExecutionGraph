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

import { ISink } from './ISink';
import { LogEvent } from './LogEvent';
import { IFormatter } from './IFormatter';
import { LogLevel, logLevels } from './LogLevel';

/**
 * Simple console sink.
 */
export class ConsoleSink implements ISink {
  constructor(private formatter: IFormatter, private readonly minLevel: LogLevel = logLevels.DEBUG) {}

  public write(event: LogEvent) {
    if (!event.level.isGreaterThanOrEqualTo(this.minLevel)) {
      return;
    }

    const formattedMessage = this.formatter.format(event);

    switch (event.level) {
      case logLevels.DEBUG:
        // tslint:disable:no-console
        console.debug(formattedMessage);
        // tslint:enable:no-console
        break;
      case logLevels.INFO:
        console.log(formattedMessage);
        break;
      case logLevels.WARN:
        console.warn(formattedMessage);
        break;
      case logLevels.ERROR:
        console.error(formattedMessage);
        break;
    }
  }
}
