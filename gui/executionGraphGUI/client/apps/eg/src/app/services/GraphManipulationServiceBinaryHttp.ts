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

import { Inject } from '@angular/core';
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/first';

import { flatbuffers } from 'flatbuffers';
import { GraphManipulationService, sz } from './GraphManipulationService';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';
import { Identifier } from '@eg/comon/Identifier';
import * as D from "@eg/comon/DataTypes"

export class GraphManipulationServiceBinaryHttp extends GraphManipulationService {

  constructor(@Inject(BinaryHttpRouterService) private readonly binaryRouter: BinaryHttpRouterService, private readonly verboseLog = true) {
    super();
  }

  private readonly _id = new Identifier("GraphManipulationServiceBinaryHttp");
  public get id(): Identifier { return this._id; }

  public async addNode(graphId: string, type: string, name: string): Promise<sz.AddNodeResponse> {

    // Build the AddNode request
    let builder = new flatbuffers.Builder(356);
    let offGraphId = builder.createString(graphId);
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
    console.log(`[${this.id.name}] Added new node of type: '${node.type()}'
                  with name: '${node.name()}' [ins: ${node.inputSocketsLength()},
                  outs: ${node.outputSocketsLength()}`);

    return response;
  }

  public async removeNode(graphId: string, nodeId: D.NodeId): Promise<void> {

    // Build the RemoveNode request
    let builder = new flatbuffers.Builder(356);
    let offGraphId = builder.createString(graphId);
    sz.RemoveNodeRequest.startRemoveNodeRequest(builder);
    sz.RemoveNodeRequest.addGraphId(builder, offGraphId);
    sz.RemoveNodeRequest.addNodeId(builder, builder.createLong(nodeId.lower, nodeId.higher));
    let reqOff = sz.RemoveNodeRequest.endRemoveNodeRequest(builder);
    builder.finish(reqOff);

    let requestPayload = builder.asUint8Array();

    // Send the request
    await this.binaryRouter.post('graph/removeNode', requestPayload);

  }
}
