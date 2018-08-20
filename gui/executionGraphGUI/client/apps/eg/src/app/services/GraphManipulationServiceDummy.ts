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

import { Injectable } from "@angular/core";
import { flatbuffers } from "flatbuffers";
import { ILogger, LoggerFactory } from "@eg/logger";
import { Id } from "@eg/common";
import { GraphManipulationService, sz } from "./GraphManipulationService";
import { toNode } from "./Conversions";
import { Node, NodeId } from "../model";

@Injectable()
export class GraphManipulationServiceDummy extends GraphManipulationService {
  private logger: ILogger;

  constructor(loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create("GraphManipulationServiceDummy");
  }

  public async addNode(graphId: Id, type: string, name: string): Promise<Node> {
    // Build the AddNode request
    let builder = new flatbuffers.Builder(345);
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

    // Build a dummy result
    builder = new flatbuffers.Builder(1024);
    let nameOff = builder.createString(name);
    let typeOff = builder.createString(type);
    sz.LogicNode.startLogicNode(builder);
    sz.LogicNode.addId(builder, builder.createLong(13, 13));
    sz.LogicNode.addName(builder, nameOff);
    sz.LogicNode.addType(builder, typeOff);
    let nodeOff = sz.LogicNode.endLogicNode(builder);

    sz.AddNodeResponse.startAddNodeResponse(builder);
    sz.AddNodeResponse.addNode(builder, nodeOff);
    let respOff = sz.AddNodeResponse.endAddNodeResponse(builder);
    builder.finish(respOff);
    let result = builder.asUint8Array();

    const buf = new flatbuffers.ByteBuffer(result);
    const response = sz.AddNodeResponse.getRootAsAddNodeResponse(buf);

    let node = response.node();
    this.logger.info(`Added new node [type: '${node.type()}']
                  with name: '${node.name()}' [ins: ${node.inputSocketsLength()},
                  outs: ${node.outputSocketsLength()}`);

    return toNode(node);
  }

  public async removeNode(graphId: Id, nodeId: NodeId): Promise<void> {
    this.logger.error(`Not implemented yet!`);
  }
}
