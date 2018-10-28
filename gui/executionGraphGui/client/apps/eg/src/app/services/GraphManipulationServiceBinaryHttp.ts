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
import { Id } from '@eg/common';
import { GraphManipulationService, sz } from './GraphManipulationService';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';
import { Node, NodeId, Socket, Connection } from '../model';
import * as conversions from './Conversions';
import { isDefined } from '@angular/compiler/src/util';

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

  public async addNode(graphId: Id, type: string, name: string): Promise<Node> {
    // Build the AddNode request
    let builder = new flatbuffers.Builder(356);
    let offGraphId = builder.createString(graphId.toString());
    let offType = builder.createString(type);
    let offName = builder.createString(name);

    sz.NodeConstructionInfo.startNodeConstructionInfo(builder);
    sz.NodeConstructionInfo.addName(builder, offName);
    sz.NodeConstructionInfo.addType(builder, offType);
    let off = sz.NodeConstructionInfo.endNodeConstructionInfo(builder);

    sz.AddNodeRequest.startAddNodeRequest(builder);
    sz.AddNodeRequest.addGraphId(builder, offGraphId);
    sz.AddNodeRequest.addNode(builder, off);
    off = sz.AddNodeRequest.endAddNodeRequest(builder);
    builder.finish(off);

    let requestPayload = builder.asUint8Array();

    // Send the request
    const result = await this.binaryRouter.post('graph/addNode', requestPayload);
    const buf = new flatbuffers.ByteBuffer(result);
    const response = sz.AddNodeResponse.getRootAsAddNodeResponse(buf);

    let node = response.node();
    this.logger.info(`Added new node [type: '${node.type()}', name: '${node.name()}', ins: ${node.inputSocketsLength()},
                  outs: ${node.outputSocketsLength()}].`);

    let nodeModel = conversions.toNode(node);
    if (this.verboseResponseLog) {
      this.logger.info(`Node: '${stringify(nodeModel)}'`);
    }
    return nodeModel;
  }

  public async removeNode(graphId: Id, nodeId: NodeId): Promise<void> {
    // Build the RemoveNode request
    let builder = new flatbuffers.Builder(356);
    let offGraphId = builder.createString(graphId.toString());
    sz.RemoveNodeRequest.startRemoveNodeRequest(builder);
    sz.RemoveNodeRequest.addGraphId(builder, offGraphId);
    sz.RemoveNodeRequest.addNodeId(builder, builder.createLong(nodeId.low, nodeId.high));
    let reqOff = sz.RemoveNodeRequest.endRemoveNodeRequest(builder);
    builder.finish(reqOff);

    let requestPayload = builder.asUint8Array();

    // Send the request
    await this.binaryRouter.post('graph/removeNode', requestPayload);
    this.logger.debug(`Removed node [id: '${nodeId}'] from graph [id: '${graphId.toString()}']`);
  }

  public async addConnection(
    graphId: Id,
    source: Socket,
    target: Socket,
    cycleDetection: boolean
  ): Promise<Connection> {
    const connection = Connection.create(source, target, true);

    const builder = new flatbuffers.Builder(356);
    const offGraphId = builder.createString(graphId.toString());

    sz.AddConnectionRequest.startAddConnectionRequest(builder);
    sz.AddConnectionRequest.addGraphId(builder, offGraphId);
    sz.AddConnectionRequest.addSocketLink(
      builder,
      sz.SocketLinkDescription.createSocketLinkDescription(
        builder,
        conversions.toFbLong(connection.outputSocket.parent.id),
        conversions.toFbLong(connection.outputSocket.index),
        conversions.toFbLong(connection.inputSocket.parent.id),
        conversions.toFbLong(connection.inputSocket.index),
        connection.isWriteLink
      )
    );
    sz.AddConnectionRequest.addCheckForCycles(builder, cycleDetection);
    sz.AddConnectionRequest.endAddConnectionRequest(builder);

    const requestPayload = builder.asUint8Array();

    const result = await this.binaryRouter.post('graph/addConnection', requestPayload);

    if (!result.length) {
      // Succesfully added connection
      this.logger.info(
        `Added connection: ['${source.idString}' ⟶ '${target.idString}'] from graph [id: '${graphId.toString()}']`
      );

      return connection;
    } else {
      // Cycle detected, read the repsonse
      const buf = new flatbuffers.ByteBuffer(result);
      const response = sz.AddNodeResponse.getRootAsAddNodeResponse(buf);
      throw 'Cycle detection not yet implemented!';
    }
  }

  public async removeConnection(graphId: Id, source: Socket, target: Socket): Promise<void> {
    const connection = Connection.create(source, target, false);

    const builder = new flatbuffers.Builder(356);
    const offGraphId = builder.createString(graphId.toString());

    sz.RemoveConnectionRequest.startRemoveConnectionRequest(builder);
    sz.RemoveConnectionRequest.addGraphId(builder, offGraphId);
    sz.RemoveConnectionRequest.addSocketLink(
      builder,
      sz.SocketLinkDescription.createSocketLinkDescription(
        builder,
        conversions.toFbLong(connection.outputSocket.parent.id),
        conversions.toFbLong(connection.outputSocket.index),
        conversions.toFbLong(connection.inputSocket.parent.id),
        conversions.toFbLong(connection.inputSocket.index),
        connection.isWriteLink
      )
    );
    sz.RemoveConnectionRequest.endRemoveConnectionRequest(builder);

    const requestPayload = builder.asUint8Array();

    await this.binaryRouter.post('graph/removeConnection', requestPayload);
    this.logger.info(
      `Removed connection: ['${source.idString}' ⟶ '${target.idString}'] from graph [id: '${graphId.toString()}']`
    );
  }
}
