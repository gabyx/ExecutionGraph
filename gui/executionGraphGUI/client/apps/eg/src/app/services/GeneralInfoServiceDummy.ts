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
import { GeneralInfoService, sz} from './GeneralInfoService';
import { flatbuffers } from 'flatbuffers';

@Injectable()
export class GeneralInfoServiceDummy extends GeneralInfoService {

  constructor() {
    super();
  }

  public async getAllGraphTypeDescriptions(): Promise<sz.GetAllGraphTypeDescriptionsResponse> {
    let builder = new flatbuffers.Builder(1024);

    // NodeType Description machen
    let offName = builder.createString("DummyNode");
    let offType = builder.createString("DummyNode<int>");
    sz.NodeTypeDescription.startNodeTypeDescription(builder);
    sz.NodeTypeDescription.addName(builder, offName);
    sz.NodeTypeDescription.addType(builder, offType);
    let offN = sz.NodeTypeDescription.endNodeTypeDescription(builder);

    // SocketType Description machen
    offName = builder.createString("Integral Socket");
    offType = builder.createString("int");
    sz.SocketTypeDescription.startSocketTypeDescription(builder);
    sz.SocketTypeDescription.addName(builder, offName);
    sz.SocketTypeDescription.addType(builder, offType);
    let offS = sz.SocketTypeDescription.endSocketTypeDescription(builder);

    // GraphType Description machen
    let offId = builder.createString("2992ebff-c950-4184-8876-5fe6ac029aa5");
    offName = builder.createString("DefaultGraph");
    let offNodes = sz.GraphTypeDescription.createNodeTypeDescriptionsVector(builder, [offN]);
    let offSockets = sz.GraphTypeDescription.createSocketTypeDescriptionsVector(builder, [offS]);
    sz.GraphTypeDescription.startGraphTypeDescription(builder);
    sz.GraphTypeDescription.addId(builder, offId);
    sz.GraphTypeDescription.addName(builder, offName);
    sz.GraphTypeDescription.addNodeTypeDescriptions(builder, offNodes);
    sz.GraphTypeDescription.addSocketTypeDescriptions(builder, offSockets);
    let offGT = sz.GraphTypeDescription.endGraphTypeDescription(builder);

    // GraphType Descriptions machen
    let offGTs = sz.GetAllGraphTypeDescriptionsResponse.createGraphsTypesVector(builder, [offGT]);
    sz.GetAllGraphTypeDescriptionsResponse.startGetAllGraphTypeDescriptionsResponse(builder);
    sz.GetAllGraphTypeDescriptionsResponse.addGraphsTypes(builder, offGTs);
    let offResp = sz.GetAllGraphTypeDescriptionsResponse.endGetAllGraphTypeDescriptionsResponse(builder);
    builder.finish(offResp);

    let buf = new flatbuffers.ByteBuffer(builder.asUint8Array());
    let response = sz.GetAllGraphTypeDescriptionsResponse.getRootAsGetAllGraphTypeDescriptionsResponse(buf);

    console.debug(`[GeneralInfoServiceBinaryHttp] Received: Number of Graph types: ${response.graphsTypesLength()}`)
    return response;
  }


}
