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
import { Id } from '@eg/common';
import * as conversions from './Conversions';
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
  public graphTypeDescs: model.GraphTypeDescription[] = [];
  public graphTypeDescsResponse: szInfo.GetAllGraphTypeDescriptionsResponse;
  public abstract createAddNodeResponse(type: string, name: string): szMani.AddNodeResponse;
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
  private nodeId: number = 0;
  private graphTypeId: Id;
  private sockets: string[];
  private nodes: {
    [type: string]: {
      type: string;
      name: string;
      ins: string[];
      outs: string[];
    };
  };

  public graphTypeDesc: model.GraphTypeDescription;

  constructor() {
    super();

    this.graphTypeId = new Id('2992ebff-c950-4184-8876-5fe6ac029aa5');

    // Internal sockets
    this.sockets = ['int', 'bool', 'double', 'Vector3', 'Matrix33', 'Scalar'];

    // Internal node map
    this.nodes = {
      Vector3: { type: 'Vector3', name: 'Vector3', ins: [], outs: ['Vector3'] },
      Scalar: { type: 'Scalar', name: 'Scalar', ins: [], outs: ['Scalar'] },
      Matrix33: { type: 'Matrix33', name: 'Matrix33', ins: [], outs: ['Matrix33'] },

      Addition: { type: 'Addition', name: 'Addition', ins: ['Vector3', 'Vector3'], outs: ['Vector3'] },
      Subtraction: { type: 'Subtraction', name: 'Subtraction', ins: ['Vector3', 'Vector3'], outs: ['Vector3'] },

      CrossProduct: { type: 'CrossProduct', name: 'CrossProduct', ins: ['Vector3', 'Vector3'], outs: ['Vector3'] },
      ScalarProduct: { type: 'ScalarProduct', name: 'ScalarProduct', ins: ['Vector3', 'Vector3'], outs: ['Scalar'] },
      Tildify: { type: 'Tildify', name: 'Tildify', ins: ['Vector3'], outs: ['Matrix33'] },
      Normalize: { type: 'Normalize', name: 'Normalize', ins: ['Vector3'], outs: ['Vector3'] },

      LinearTransform: {
        type: 'LinearTransform',
        name: 'LinearTransform',
        ins: ['Matrix33', 'Vector3'],
        outs: ['Vector3']
      }
    };

    // Generate Descriptions
    // ---------------------
    let builder = new flatbuffers.Builder(1024);

    // NodeType Description machen
    let nodeDescs = [];
    for (let type in this.nodes) {
      let n = this.nodes[type];
      let offName = builder.createString(n['name']);
      let offType = builder.createString(n['type']);
      szInfo.NodeTypeDescription.startNodeTypeDescription(builder);
      szInfo.NodeTypeDescription.addName(builder, offName);
      szInfo.NodeTypeDescription.addType(builder, offType);
      let offN = szInfo.NodeTypeDescription.endNodeTypeDescription(builder);
      nodeDescs.push(offN);
    }

    // SocketType Description machen
    let socketDescs = [];
    for (let s of this.sockets) {
      let offName = builder.createString(`[${s}]`);
      let offType = builder.createString(s);
      szInfo.SocketTypeDescription.startSocketTypeDescription(builder);
      szInfo.SocketTypeDescription.addName(builder, offName);
      szInfo.SocketTypeDescription.addType(builder, offType);
      let offS = szInfo.SocketTypeDescription.endSocketTypeDescription(builder);
      socketDescs.push(offS);
    }

    // Make GraphType Description
    let offId = builder.createString(this.graphTypeId.toString());
    let offName = builder.createString('DefaultGraph');
    let offNodes = szInfo.GraphTypeDescription.createNodeTypeDescriptionsVector(builder, nodeDescs);
    let offSockets = szInfo.GraphTypeDescription.createSocketTypeDescriptionsVector(builder, socketDescs);
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

    this.graphTypeDesc = conversions.toGraphTypeDescription(response.graphsTypes(0));

    this.graphTypeDescs.push(this.graphTypeDesc);
    this.graphTypeDescsResponse = response;
  }

  public createAddNodeResponse(type: string, name: string): szMani.AddNodeResponse {
    // Create a node response
    // ----------------------

    if (!(type in this.nodes)) {
      throw `TestBackend: No such node type: ${type}`;
    }

    let node = this.nodes[type];

    let builder = new flatbuffers.Builder(1024);
    let nameOff = builder.createString(name);
    let typeOff = builder.createString(node.type);

    let inSocksOff = this.createSockets(builder, node.ins, 'in', true);
    let outSocksOff = this.createSockets(builder, node.outs, 'out', false);

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

  private createSockets(builder: flatbuffers.Builder, sockets: string[], suffix: string, inputs: boolean) {
    let socketOffs: number[] = [];
    for (let i = 0; i < sockets.length; i++) {
      let type = sockets[i];
      let typeIndex = this.sockets.findIndex((v, idx) => v === type);
      if (typeIndex < 0) {
        throw `TestBackend: No such socket type ${type}`;
      }
      let socketNameOff = builder.createString(`${suffix}-${i}`);
      let typeOff = builder.createString(type);

      szMani.LogicSocket.startLogicSocket(builder);

      szMani.LogicSocket.addTypeIndex(builder, flatbuffers.Long.create(typeIndex, 0));
      szMani.LogicSocket.addType(builder, typeOff);
      szMani.LogicSocket.addTypeName(builder, typeOff);

      szMani.LogicSocket.addName(builder, socketNameOff);
      szMani.LogicSocket.addIndex(builder, flatbuffers.Long.create(i, 0));
      let off = szMani.LogicSocket.endLogicSocket(builder);
      socketOffs.push(off);
    }
    return szMani.LogicNode.createInputSocketsVector(builder, socketOffs);
  }
}
