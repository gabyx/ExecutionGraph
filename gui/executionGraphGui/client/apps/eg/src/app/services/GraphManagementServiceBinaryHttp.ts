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

import { Inject, Injectable } from '@angular/core';
import { flatbuffers } from 'flatbuffers';
import { Graph } from '../model';
import { ILogger, LoggerFactory } from '@eg/logger';
import { GraphManagementService, sz } from './GraphManagementService';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';
import { VERBOSE_LOG_TOKEN } from '../tokens';
import { GraphTypeId, GraphId } from '../model/Graph';

@Injectable()
export class GraphManagementServiceBinaryHttp extends GraphManagementService {
  private logger: ILogger;

  constructor(
    loggerFactory: LoggerFactory,
    private readonly binaryRouter: BinaryHttpRouterService,
    @Inject(VERBOSE_LOG_TOKEN) private readonly verboseLog = true
  ) {
    super();
    this.logger = loggerFactory.create('GraphManagementServiceBinaryHttp');
  }

  public async addGraph(graphTypeId: GraphTypeId): Promise<Graph> {
    // Build the AddGraph request
    const builder = new flatbuffers.Builder(16);
    const offGraphTypeId = builder.createString(graphTypeId);

    sz.AddGraphRequest.startAddGraphRequest(builder);
    sz.AddGraphRequest.addGraphTypeId(builder, offGraphTypeId);
    const off = sz.AddGraphRequest.endAddGraphRequest(builder);
    builder.finish(off);

    const requestPayload = builder.asUint8Array();

    // Send the request
    const result = await this.binaryRouter.post('general/addGraph', requestPayload);
    const buf = new flatbuffers.ByteBuffer(result);
    const response = sz.AddGraphResponse.getRootAsAddGraphResponse(buf);

    this.logger.info(`Added new graph [id: '${response.graphId()}', type: '${graphTypeId}'].`);

    const graph: Graph = {
      id: response.graphId(),
      typeId: graphTypeId,
      connections: {},
      nodes: {},
      name: ''
    };
    return graph;
  }

  public async removeGraph(graphId: GraphId): Promise<void> {
    // Build the RemoveGraph request
    const builder = new flatbuffers.Builder(16);
    const offGraphId = builder.createString(graphId);

    sz.RemoveGraphRequest.startRemoveGraphRequest(builder);
    sz.RemoveGraphRequest.addGraphId(builder, offGraphId);
    const off = sz.RemoveGraphRequest.endRemoveGraphRequest(builder);
    builder.finish(off);

    const requestPayload = builder.asUint8Array();

    // Send the request
    await this.binaryRouter.post('general/removeGraph', requestPayload);
    this.logger.info(`Removed graph [id: '${graphId}'].`);
  }

  public async saveGraph(graphId: GraphId, filePath: string, overwrite: boolean): Promise<void> {
    // Build the RemoveGraph request
    const builder = new flatbuffers.Builder(16);
    const offGraphId = builder.createString(graphId);
    const offFilePath = builder.createString(filePath);

    sz.SaveGraphRequest.startSaveGraphRequest(builder);
    sz.SaveGraphRequest.addGraphId(builder, offGraphId);
    sz.SaveGraphRequest.addFilePath(builder, offFilePath);
    sz.SaveGraphRequest.addOverwrite(builder, overwrite);
    const off = sz.SaveGraphRequest.endSaveGraphRequest(builder);
    builder.finish(off);

    const requestPayload = builder.asUint8Array();

    // Send the request
    await this.binaryRouter.post('general/saveGraph', requestPayload);
    this.logger.debug(`Saved graph id: '${graphId}' to file: '${filePath}'`);
  }
  public async loadGraph(path: string): Promise<Graph> {
    throw Error('Not Implemented!');
  }
}
