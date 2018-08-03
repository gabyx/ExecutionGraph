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

import { Injectable, Testability } from '@angular/core';
import { GraphInfoMessages, GeneralInfoService } from './GeneralInfoService';
import { flatbuffers } from 'flatbuffers';

@Injectable()
export class GraphInfoServiceDummy extends GeneralInfoService {

  constructor() {
    super();
  }

  public async getAllGraphTypeDescriptions(): Promise<GraphInfoMessages.GetAllGraphTypeDescriptionsResponse> {
    let builder = new flatbuffers.Builder(1024);

    // NodeType Description machen
    let offName = builder.createString("DummyNode");
    let offType = builder.createString("DummyNode<int>");
    GraphInfoMessages.NodeTypeDescription.startNodeTypeDescription(builder);
    GraphInfoMessages.NodeTypeDescription.addName(builder, offName);
    GraphInfoMessages.NodeTypeDescription.addType(builder, offType);
    let offN = GraphInfoMessages.NodeTypeDescription.endNodeTypeDescription(builder);

    // SocketType Description machen
    offName = builder.createString("Integral Socket");
    offType = builder.createString("int");
    GraphInfoMessages.SocketTypeDescription.startSocketTypeDescription(builder);
    GraphInfoMessages.SocketTypeDescription.addName(builder, offName);
    GraphInfoMessages.SocketTypeDescription.addType(builder, offType);
    let offS = GraphInfoMessages.SocketTypeDescription.endSocketTypeDescription(builder);

    // GraphType Description machen
    let offId = builder.createString("2992ebff-c950-4184-8876-5fe6ac029aa5");
    offName = builder.createString("DefaultGraph");
    let offNodes = GraphInfoMessages.GraphTypeDescription.createNodeTypeDescriptionsVector(builder, [offN]);
    let offSockets = GraphInfoMessages.GraphTypeDescription.createSocketTypeDescriptionsVector(builder, [offS]);
    GraphInfoMessages.GraphTypeDescription.startGraphTypeDescription(builder);
    GraphInfoMessages.GraphTypeDescription.addId(builder, offId);
    GraphInfoMessages.GraphTypeDescription.addName(builder, offName);
    GraphInfoMessages.GraphTypeDescription.addNodeTypeDescriptions(builder, offNodes);
    GraphInfoMessages.GraphTypeDescription.addSocketTypeDescriptions(builder, offSockets);
    let offGT = GraphInfoMessages.GraphTypeDescription.endGraphTypeDescription(builder);

    // GraphType Descriptions machen
    let offGTs = GraphInfoMessages.GetAllGraphTypeDescriptionsResponse.createGraphsTypesVector(builder, [offGT]);
    GraphInfoMessages.GetAllGraphTypeDescriptionsResponse.startGetAllGraphTypeDescriptionsResponse(builder);
    GraphInfoMessages.GetAllGraphTypeDescriptionsResponse.addGraphsTypes(builder, offGTs);
    let offResp = GraphInfoMessages.GetAllGraphTypeDescriptionsResponse.endGetAllGraphTypeDescriptionsResponse(builder);
    builder.finish(offResp);

    let buf = new flatbuffers.ByteBuffer(builder.asUint8Array());
    let response = GraphInfoMessages.GetAllGraphTypeDescriptionsResponse.getRootAsGetAllGraphTypeDescriptionsResponse(buf);

    console.debug(`[GraphInfoServiceBinaryHttp] Received: Number of Graph types: ${response.graphsTypesLength()}`)
    return response;
  }


}
