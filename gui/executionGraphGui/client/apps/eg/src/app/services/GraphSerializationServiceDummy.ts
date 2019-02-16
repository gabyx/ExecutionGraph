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
import { GraphSerializationService } from './GraphSerializationService';
import { Graph, GraphTypeId, GraphId } from '../model';
import { Guid } from 'guid-typescript';
import { ITestBackend } from './TestBackend';

@Injectable()
export class GraphSerializationServiceDummy extends GraphSerializationService {
  private logger: ILogger;

  constructor(loggerFactory: LoggerFactory, private readonly backend: ITestBackend) {
    super();
    this.logger = loggerFactory.create('GraphSerializationServiceDummy');
  }

  public async saveGraph(graphId: GraphId, filePath: string, overwrite: boolean): Promise<void> {
    this.logger.debug(`Saving graph id: '${graphId}' to file: '${filePath}'`);
  }
  public async loadGraph(filePath: string): Promise<Graph> {
    this.logger.debug(`Loading graph from file: '${filePath}'`);
    return {
      id: Guid.create().toString(),
      name: 'Unnamed',
      connections: {},
      nodes: {},
      typeId: this.backend.graphTypeDescs[0].id
    };
  }
}
