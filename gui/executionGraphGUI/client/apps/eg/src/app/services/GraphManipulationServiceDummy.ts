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

import { Injectable, Inject } from '@angular/core';
import { VERBOSE_LOG_TOKEN } from '../tokens';
import { ITestBackend } from './TestBackend';
import { ILogger, LoggerFactory, stringify } from '@eg/logger';
import { Id } from '@eg/common';
import { GraphManipulationService, sz } from './GraphManipulationService';
import { toNode } from './Conversions';
import { Node, NodeId } from '../model';

@Injectable()
export class GraphManipulationServiceDummy extends GraphManipulationService {
  private logger: ILogger;
  private nodeId: number = 0;

  constructor(
    loggerFactory: LoggerFactory,
    private backend: ITestBackend,
    @Inject(VERBOSE_LOG_TOKEN) private readonly verboseResponseLog = true
  ) {
    super();
    this.logger = loggerFactory.create('GraphManipulationServiceDummy');
  }

  public async addNode(graphId: Id, type: string, name: string): Promise<Node> {
    let response = this.backend.createAddNodeResponse(type, name);

    let node = response.node();
    this.logger.info(`Added new node [type: '${node.type()}']
                  with name: '${node.name()}' [ins: ${node.inputSocketsLength()},
                  outs: ${node.outputSocketsLength()}`);

    let nodeModel = toNode(node);
    if (this.verboseResponseLog) {
      this.logger.info(`Node: '${stringify(nodeModel)}'`);
    }
    return nodeModel;
  }

  public async removeNode(graphId: Id, nodeId: NodeId): Promise<void> {
    this.logger.info(`Remove node [id: '${nodeId.toString()}'] from graph id '${graphId.toString()}'`);
  }
}
