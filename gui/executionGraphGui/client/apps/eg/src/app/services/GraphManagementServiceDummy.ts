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
import { ILogger, LoggerFactory } from '@eg/logger';
import { GraphManagementService } from './GraphManagementService';
import { ITestBackend } from './TestBackend';
import { Graph, GraphTypeId, GraphId } from '../model';
import { Guid } from 'guid-typescript';

@Injectable()
export class GraphManagementServiceDummy extends GraphManagementService {
  private logger: ILogger;

  constructor(loggerFactory: LoggerFactory, private backend: ITestBackend) {
    super();
    this.logger = loggerFactory.create('GraphManagementServiceDummy');
  }

  public async addGraph(graphTypeId: GraphTypeId): Promise<Graph> {
    return {
      id: Guid.create().toString(),
      typeId: graphTypeId,
      nodes: {},
      connections: {},
      name: 'MyDummyGraph'
    };
  }

  public async removeGraph(graphId: GraphId): Promise<void> {}

  public async saveGraph(graphId: GraphId, path: string, overwrite: boolean): Promise<void> {
    this.logger.debug(`Saving graph id: '${graphId}' to file: '${path}'`);
  }
  public async loadGraph(path: string): Promise<Graph> {
    throw new Error('Not Implemented!');
  }
}
