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
import { flatbuffers } from 'flatbuffers';
import { ILogger, LoggerFactory, stringify } from '@eg/logger';
import { GraphManipulationService, sz } from './GraphManipulationService';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';
import { Node, NodeId, Socket, Connection, fromConnection } from '../model';
import { GraphId } from '../model/Graph';
import { toFbLong, toNode } from './Conversions';

@Injectable()
export class GraphManipulationServiceBinaryHttp extends GraphManipulationService {
  private logger: ILogger;

  constructor(
    loggerFactory: LoggerFactory,
    private readonly binaryRouter: BinaryHttpRouterService,
    @Inject(VERBOSE_LOG_TOKEN) private readonly verboseResponseLog = true
  ) {
    super();
    this.logger = loggerFactory.create('GraphManipulationServiceBinaryHttp');
  }

  public async addNode(graphId: GraphId, type: string): Promise<Node> {
    // Build the AddNode request
    const builder = new flatbuffers.Builder(356);
    const offGraphId = builder.createString(graphId);
    const offType = builder.createString(type);
    const offName = builder.createString(name);

    sz.NodeConstructionInfo.start(builder);
    sz.NodeConstructionInfo.addName(builder, offName);
    sz.NodeConstructionInfo.addType(builder, offType);
    let off = sz.NodeConstructionInfo.end(builder);

    sz.AddNodeRequest.start(builder);
    sz.AddNodeRequest.addGraphId(builder, offGraphId);
    sz.AddNodeRequest.addNode(builder, off);
    off = sz.AddNodeRequest.end(builder);
    builder.finish(off);

    const requestPayload = builder.asUint8Array();

    // Send the request
    const result = await this.binaryRouter.post('graph/addNode', requestPayload);
    const buf = new flatbuffers.ByteBuffer(result);
    const response = sz.AddNodeResponse.getRoot(buf);

    const node = response.node();
    this.logger.info(`Added new node [type: '${node.type()}', ins: ${node.inputSocketsLength()},
                  outs: ${node.outputSocketsLength()}].`);

    const nodeModel = toNode(node);
    if (this.verboseResponseLog) {
      this.logger.info(`Node: '${stringify(nodeModel, 4)}'`);
    }
    return nodeModel;
  }

  public async removeNode(graphId: GraphId, nodeId: NodeId): Promise<void> {
    // Build the RemoveNode request
    const builder = new flatbuffers.Builder(356);
    const offGraphId = builder.createString(graphId);
    sz.RemoveNodeRequest.start(builder);
    sz.RemoveNodeRequest.addGraphId(builder, offGraphId);
    sz.RemoveNodeRequest.addNodeId(builder, toFbLong(nodeId));
    const reqOff = sz.RemoveNodeRequest.end(builder);
    builder.finish(reqOff);

    const requestPayload = builder.asUint8Array();

    // Send the request
    await this.binaryRouter.post('graph/removeNode', requestPayload);
    this.logger.debug(`Removed node [id: '${nodeId}'] from graph [id: '${graphId}']`);
  }

  public async addConnection(
    graphId: GraphId,
    source: Socket,
    target: Socket,
    cycleDetection: boolean
  ): Promise<Connection> {
    const connection = fromConnection.createValidConnection(source, target);

    const builder = new flatbuffers.Builder(356);
    const offGraphId = builder.createString(graphId);

    sz.AddConnectionRequest.start(builder);
    sz.AddConnectionRequest.addGraphId(builder, offGraphId);
    sz.AddConnectionRequest.addSocketLink(
      builder,
      sz.SocketLinkDescription.create(
        builder,
        toFbLong(connection.outputSocket.parentId),
        toFbLong(connection.outputSocket.index),
        toFbLong(connection.inputSocket.parentId),
        toFbLong(connection.inputSocket.index),
        connection.isWriteLink
      )
    );
    sz.AddConnectionRequest.addCheckForCycles(builder, cycleDetection);
    const off = sz.AddConnectionRequest.end(builder);
    builder.finish(off);

    const requestPayload = builder.asUint8Array();

    const result = await this.binaryRouter.post('graph/addConnection', requestPayload);

    if (!result.length) {
      // Succesfully added connection
      this.logger.info(`Added connection: ['${source.id}' ⟶ '${target.id}'] from graph [id: '${graphId}']`);

      return connection;
    } else {
      // Cycle detected, read the repsonse
      const buf = new flatbuffers.ByteBuffer(result);
      const response = sz.AddNodeResponse.getRoot(buf);
      throw new Error('Cycle detection not yet implemented!');
    }
  }

  public async removeConnection(graphId: GraphId, source: Socket, target: Socket): Promise<void> {
    const connection = fromConnection.createConnection(source, target);

    const builder = new flatbuffers.Builder(356);
    const offGraphId = builder.createString(graphId);

    sz.RemoveConnectionRequest.start(builder);
    sz.RemoveConnectionRequest.addGraphId(builder, offGraphId);
    sz.RemoveConnectionRequest.addSocketLink(
      builder,
      sz.SocketLinkDescription.create(
        builder,
        toFbLong(connection.outputSocket.parentId),
        toFbLong(connection.outputSocket.index),
        toFbLong(connection.inputSocket.parentId),
        toFbLong(connection.inputSocket.index),
        connection.isWriteLink
      )
    );
    const off = sz.RemoveConnectionRequest.end(builder);
    builder.finish(off);

    const requestPayload = builder.asUint8Array();

    await this.binaryRouter.post('graph/removeConnection', requestPayload);
    this.logger.info(`Removed connection: ['${source.id}' ⟶ '${target.id}'] from graph [id: '${graphId}']`);
  }
}
