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
import * as conversions from './Conversions';
import { Node, NodeId, InputSocket, OutputSocket, Connection } from '../model';

@Injectable()
export class GraphManipulationServiceDummy extends GraphManipulationService {
  private logger: ILogger;

  constructor(
    loggerFactory: LoggerFactory,
    private backend: ITestBackend,
    @Inject(VERBOSE_LOG_TOKEN) private readonly verboseResponseLog = true
  ) {
    super();
    this.logger = loggerFactory.create('GraphManipulationServiceDummy');
  }

  public async addNode(graphId: Id, type: string, name: string): Promise<Node> {
    const response = this.backend.createAddNodeResponse(type, name);

    const node = response.node();
    if (this.verboseResponseLog) {
      this.logger.info(`Added new node [type: '${node.type()}']
                    with name: '${node.name()}' [ins: ${node.inputSocketsLength()},
                    outs: ${node.outputSocketsLength()}`);
    }

    const nodeModel = conversions.toNode(node);
    if (this.verboseResponseLog) {
      this.logger.info(`Node: '${stringify(nodeModel)}'`);
    }
    return nodeModel;
  }

  public async removeNode(graphId: Id, nodeId: NodeId): Promise<void> {
    this.logger.info(`Remove node [id: '${nodeId.toString()}'] from graph id '${graphId.toString()}'`);
  }

  public async addConnection(
    graphId: Id,
    outputSocket: OutputSocket,
    inputSocket: InputSocket,
    isWriteLink: boolean,
    cycleDetection: boolean
  ): Promise<Connection> {
    return Connection.create(outputSocket, inputSocket, true);
  }

  public async removeConnection(
    graphId: Id,
    outputSocket: OutputSocket,
    inputSocket: InputSocket,
    isWriteLink: boolean
  ): Promise<void> {
    this.logger.info(
      `Remove connection: '${outputSocket.idString}'` + isWriteLink
        ? `⟵`
        : `⟶` + `'${inputSocket.idString}' from graph id '${graphId.toString}'`
    );
  }
}
