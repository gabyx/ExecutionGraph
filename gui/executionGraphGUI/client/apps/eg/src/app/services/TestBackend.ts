// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Mon Sep 24 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { Injectable } from '@angular/core';
import { flatbuffers } from 'flatbuffers';
import { Id } from "@eg/common";
import { toGraphTypeDescription } from './Conversions';
import { sz as szInfo } from './GeneralInfoService';
import { sz as szMani } from './GraphManipulationService';
import * as model from '../model';


/**
 * Interface class for a dummy backend which is used in the dummy services.
 *
 * @export
 * @abstract
 * @class ITestBackend
 */
export abstract class ITestBackend {
  public graphTypeId: Id;
  public graphTypeDesc: model.GraphTypeDescription[] = [];
  public graphTypeDescsResponse: szInfo.GetAllGraphTypeDescriptionsResponse;
  public abstract createAddNodeResponse(type: string, name: string): szMani.AddNodeResponse
}

/**
 * A dummy test backend which provides a central storage only
 * for testing purposes. Providing serialized messages such that
 * the conversion pipeline can be tested in the various services.
 *
 * @export
 * @class TestBackend
 * @extends {ITestBackend}
 */
@Injectable()
export class TestBackend extends ITestBackend {
  public nodeId: number = 0;

  constructor() {
    super();

    this.graphTypeId = new Id('2992ebff-c950-4184-8876-5fe6ac029aa5');

    // Generate Descriptions
    // ---------------------
    let builder = new flatbuffers.Builder(1024);

    // NodeType Description machen
    let offName = builder.createString('DummyNode');
    let offType = builder.createString('DummyNode<int>');
    szInfo.NodeTypeDescription.startNodeTypeDescription(builder);
    szInfo.NodeTypeDescription.addName(builder, offName);
    szInfo.NodeTypeDescription.addType(builder, offType);
    let offN = szInfo.NodeTypeDescription.endNodeTypeDescription(builder);

    // SocketType Description machen
    offName = builder.createString('[int]');
    offType = builder.createString('int');
    szInfo.SocketTypeDescription.startSocketTypeDescription(builder);
    szInfo.SocketTypeDescription.addName(builder, offName);
    szInfo.SocketTypeDescription.addType(builder, offType);
    let offS = szInfo.SocketTypeDescription.endSocketTypeDescription(builder);

    // Make GraphType Description
    let offId = builder.createString(this.graphTypeId.toString());
    offName = builder.createString('DefaultGraph');
    let offNodes = szInfo.GraphTypeDescription.createNodeTypeDescriptionsVector(builder, [offN]);
    let offSockets = szInfo.GraphTypeDescription.createSocketTypeDescriptionsVector(builder, [offS]);
    szInfo.GraphTypeDescription.startGraphTypeDescription(builder);
    szInfo.GraphTypeDescription.addId(builder, offId);
    szInfo.GraphTypeDescription.addName(builder, offName);
    szInfo.GraphTypeDescription.addNodeTypeDescriptions(builder, offNodes);
    szInfo.GraphTypeDescription.addSocketTypeDescriptions(builder, offSockets);
    let offGT = szInfo.GraphTypeDescription.endGraphTypeDescription(builder);

    // Make Response
    let offGTs = szInfo.GetAllGraphTypeDescriptionsResponse.createGraphsTypesVector(builder, [offGT]);
    szInfo.GetAllGraphTypeDescriptionsResponse.startGetAllGraphTypeDescriptionsResponse(builder);
    szInfo.GetAllGraphTypeDescriptionsResponse.addGraphsTypes(builder, offGTs);
    let offResp = szInfo.GetAllGraphTypeDescriptionsResponse.endGetAllGraphTypeDescriptionsResponse(builder);
    builder.finish(offResp);

    let buf = new flatbuffers.ByteBuffer(builder.asUint8Array());
    let response = szInfo.GetAllGraphTypeDescriptionsResponse.getRootAsGetAllGraphTypeDescriptionsResponse(buf);

    for (let g = 0; g < response.graphsTypesLength(); ++g) {
      this.graphTypeDesc.push(toGraphTypeDescription(response.graphsTypes(g)));
    }
    this.graphTypeDescsResponse = response;

  }

  public createAddNodeResponse(type: string, name: string): szMani.AddNodeResponse {

    // Create a node response
    // ----------------------

    let builder = new flatbuffers.Builder(1024);
    let nameOff = builder.createString(name);
    let typeOff = builder.createString(type);

    // Create input/output sockets
    let createSockets = (nSockets: number, suffix: string, inputs: boolean) => {
      let socketOffs: number[] = [];
      for (let i = 0; i < nSockets; i++) {
        let socketType = 0;
        let sockTOff: number;
        if (inputs) {
          sockTOff = builder.createString(`${this.graphTypeDesc[0].socketTypeDescriptions[socketType].name} : ${suffix}-${i}`);
        }
        else {
          sockTOff = builder.createString(`${suffix}-${i} : ${this.graphTypeDesc[0].socketTypeDescriptions[socketType].name}`);
        }
        szMani.LogicSocket.startLogicSocket(builder);
        szMani.LogicSocket.addType(builder, flatbuffers.Long.create(socketType, 0));
        szMani.LogicSocket.addName(builder, sockTOff);
        szMani.LogicSocket.addIndex(builder, flatbuffers.Long.create(i, 0));
        let off = szMani.LogicSocket.endLogicSocket(builder);
        socketOffs.push(off);
      }
      return szMani.LogicNode.createInputSocketsVector(builder, socketOffs);
    };

    let inSocksOff = createSockets(2, "in", true);
    let outSocksOff = createSockets(3, "out", false);

    szMani.LogicNode.startLogicNode(builder);
    this.nodeId += 1;
    szMani.LogicNode.addId(builder, builder.createLong(this.nodeId, 0));
    szMani.LogicNode.addName(builder, nameOff);
    szMani.LogicNode.addType(builder, typeOff);
    szMani.LogicNode.addInputSockets(builder, inSocksOff);
    szMani.LogicNode.addOutputSockets(builder, outSocksOff);
    let nodeOff = szMani.LogicNode.endLogicNode(builder);

    szMani.AddNodeResponse.startAddNodeResponse(builder);
    szMani.AddNodeResponse.addNode(builder, nodeOff);
    let respOff = szMani.AddNodeResponse.endAddNodeResponse(builder);
    builder.finish(respOff);
    let result = builder.asUint8Array();

    const buf = new flatbuffers.ByteBuffer(result);
    return szMani.AddNodeResponse.getRootAsAddNodeResponse(buf);
  }

}
