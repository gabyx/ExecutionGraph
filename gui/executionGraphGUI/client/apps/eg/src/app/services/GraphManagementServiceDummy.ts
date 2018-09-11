// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jul 29 2018
//  @author Simon Spoerri, simon (døt) spoerri (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { Injectable } from '@angular/core';
import { GraphManagementService } from './GraphManagementService';
import { ILogger, LoggerFactory } from '@eg/logger';
import { Id } from '@eg/common';

@Injectable()
export class GraphManagementServiceDummy extends GraphManagementService {
  private logger: ILogger;

  constructor(loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('GraphManagementServiceDummy');
  }

  public async addGraph(graphTypeId: Id): Promise<Id> {
    return new Id('606b5c8f-c9fd-41e2-9ed5-6f6429fd52ba');
  }

  public async removeGraph(graphId: Id): Promise<void> {
  }
}
