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
import { Id } from "@eg/common";
import { ILogger, LoggerFactory } from '@eg/logger';
import { GraphManagementService } from './GraphManagementService';
import { ITestBackend } from './TestBackend';
import { Graph } from "../model";

@Injectable()
export class GraphManagementServiceDummy extends GraphManagementService {
  private logger: ILogger;

  constructor(
    loggerFactory: LoggerFactory,
    private backend: ITestBackend) {
    super();
    this.logger = loggerFactory.create('GraphManagementServiceDummy');
  }

  public async addGraph(graphTypeId: Id): Promise<Graph> {
    return new Graph(new Id(), this.backend.graphTypeId, [], []);
  }

  public async removeGraph(graphId: Id): Promise<void> {
  }
}
