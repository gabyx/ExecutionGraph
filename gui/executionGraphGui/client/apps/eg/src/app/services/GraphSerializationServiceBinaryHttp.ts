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
import { GraphSerializationService, sz } from './GraphSerializationService';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';
import { VERBOSE_LOG_TOKEN } from '../tokens';
import { GraphId } from '../model/Graph';
import { toGraph } from './Conversions';

@Injectable()
export class GraphSerializationServiceBinaryHttp extends GraphSerializationService {
  private logger: ILogger;

  constructor(
    loggerFactory: LoggerFactory,
    private readonly binaryRouter: BinaryHttpRouterService,
    @Inject(VERBOSE_LOG_TOKEN) private readonly verboseLog = true
  ) {
    super();
    this.logger = loggerFactory.create('GraphSerializationServiceBinaryHttp');
  }

  public async saveGraph(graphId: GraphId, filePath: string, overwrite: boolean): Promise<void> {
    this.logger.debug(`Saving graph from '${filePath}'`);

    // Build the RemoveGraph request
    const builder = new flatbuffers.Builder(16);
    const offGraphId = builder.createString(graphId);
    const offFilePath = builder.createString(filePath);

    sz.SaveGraphRequest.start(builder);
    sz.SaveGraphRequest.addGraphId(builder, offGraphId);
    sz.SaveGraphRequest.addFilePath(builder, offFilePath);
    sz.SaveGraphRequest.addOverwrite(builder, overwrite);
    const off = sz.SaveGraphRequest.end(builder);
    builder.finish(off);

    const requestPayload = builder.asUint8Array();

    // Send the request
    await this.binaryRouter.post('general/saveGraph', requestPayload);
  }
  public async loadGraph(filePath: string): Promise<Graph> {
    this.logger.debug(`Loading graph from '${filePath}'`);

    // Build the RemoveGraph request
    const builder = new flatbuffers.Builder(16);
    const offFilePath = builder.createString(filePath);

    sz.LoadGraphRequest.start(builder);
    sz.LoadGraphRequest.addFilePath(builder, offFilePath);
    const off = sz.SaveGraphRequest.end(builder);
    builder.finish(off);

    const requestPayload = builder.asUint8Array();

    // Send the request
    const result = await this.binaryRouter.post('general/loadGraph', requestPayload);

    // Load graph.
    const buf = new flatbuffers.ByteBuffer(result);
    const response = sz.LoadGraphResponse.getRoot(buf);

    // @todo gabnue->gabnue,simspoe
    // Parse in the visualization flatbuffer -> return the UI Props for the
    // graph and setup all stuff in the effect/reducer ...

    return toGraph(response.graphId(), response.graph());
  }
}
