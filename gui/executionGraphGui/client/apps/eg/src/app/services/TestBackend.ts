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
  private graphTypeId: model.GraphTypeId;
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

    this.graphTypeId = '2992ebff-c950-4184-8876-5fe6ac029aa5';

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
    const builder = new flatbuffers.Builder(1024);

    // NodeType Description machen
    const nodeDescs = [];
    Object.keys(this.nodes).map(type => {
      const n = this.nodes[type];
      const nameOff = builder.createString(n['name']);
      const offType = builder.createString(n['type']);
      szInfo.NodeTypeDescription.start(builder);
      szInfo.NodeTypeDescription.addName(builder, nameOff);
      szInfo.NodeTypeDescription.addType(builder, offType);
      const offN = szInfo.NodeTypeDescription.end(builder);
      nodeDescs.push(offN);
    });

    // SocketType Description machen
    const socketDescs = [];
    for (const s of this.sockets) {
      const offN = builder.createString(s);
      const offType = builder.createString(s);
      szInfo.SocketTypeDescription.start(builder);
      szInfo.SocketTypeDescription.addName(builder, offN);
      szInfo.SocketTypeDescription.addType(builder, offType);
      const offS = szInfo.SocketTypeDescription.end(builder);
      socketDescs.push(offS);
    }

    // Make GraphType Description

    const offId = builder.createString(this.graphTypeId);
    const offName = builder.createString('DefaultGraph');
    const offNodes = szInfo.GraphTypeDescription.createNodeTypeDescriptionsVector(builder, nodeDescs);
    const offSockets = szInfo.GraphTypeDescription.createSocketTypeDescriptionsVector(builder, socketDescs);
    szInfo.GraphTypeDescription.start(builder);
    szInfo.GraphTypeDescription.addId(builder, offId);
    szInfo.GraphTypeDescription.addName(builder, offName);
    szInfo.GraphTypeDescription.addNodeTypeDescriptions(builder, offNodes);
    szInfo.GraphTypeDescription.addSocketTypeDescriptions(builder, offSockets);
    const offGT = szInfo.GraphTypeDescription.end(builder);

    // Make Response
    const offGTs = szInfo.GetAllGraphTypeDescriptionsResponse.createGraphsTypesVector(builder, [offGT]);
    szInfo.GetAllGraphTypeDescriptionsResponse.start(builder);
    szInfo.GetAllGraphTypeDescriptionsResponse.addGraphsTypes(builder, offGTs);
    const offResp = szInfo.GetAllGraphTypeDescriptionsResponse.end(builder);
    builder.finish(offResp);

    const buf = new flatbuffers.ByteBuffer(builder.asUint8Array());
    const response = szInfo.GetAllGraphTypeDescriptionsResponse.getRootAsGetAllGraphTypeDescriptionsResponse(buf);

    this.graphTypeDesc = conversions.toGraphTypeDescription(response.graphsTypes(0));

    this.graphTypeDescs.push(this.graphTypeDesc);
    this.graphTypeDescsResponse = response;
  }

  public createAddNodeResponse(type: string, name: string): szMani.AddNodeResponse {
    // Create a node response
    // ----------------------

    if (!(type in this.nodes)) {
      throw new Error(`TestBackend: No such node type: ${type}`);
    }

    const node = this.nodes[type];

    const builder = new flatbuffers.Builder(1024);
    const nameOff = builder.createString(name);
    const typeOff = builder.createString(node.type);

    const inSocksOff = this.createSockets(builder, node.ins, 'in', true);
    const outSocksOff = this.createSockets(builder, node.outs, 'out', false);

    szMani.LogicNode.start(builder);
    this.nodeId += 1;
    szMani.LogicNode.addId(builder, builder.createLong(this.nodeId, 0));
    szMani.LogicNode.addType(builder, typeOff);
    szMani.LogicNode.addInputSockets(builder, inSocksOff);
    szMani.LogicNode.addOutputSockets(builder, outSocksOff);
    const nodeOff = szMani.LogicNode.end(builder);

    szMani.AddNodeResponse.start(builder);
    szMani.AddNodeResponse.addNode(builder, nodeOff);
    const respOff = szMani.AddNodeResponse.end(builder);
    builder.finish(respOff);
    const result = builder.asUint8Array();

    const buf = new flatbuffers.ByteBuffer(result);
    return szMani.AddNodeResponse.getRootAsAddNodeResponse(buf);
  }

  private createSockets(builder: flatbuffers.Builder, sockets: string[], suffix: string, inputs: boolean) {
    const socketOffs: number[] = [];
    for (let i = 0; i < sockets.length; i++) {
      const type = sockets[i];
      const typeIndex = this.sockets.findIndex((v, idx) => v === type);
      if (typeIndex < 0) {
        throw new Error(`TestBackend: No such socket type ${type}`);
      }
      const typeOff = builder.createString(type);

      szMani.LogicSocket.start(builder);

      szMani.LogicSocket.addTypeIndex(builder, flatbuffers.Long.create(typeIndex, 0));
      szMani.LogicSocket.addType(builder, typeOff);
      szMani.LogicSocket.addTypeName(builder, typeOff);
      szMani.LogicSocket.addIndex(builder, flatbuffers.Long.create(i, 0));
      const off = szMani.LogicSocket.end(builder);
      socketOffs.push(off);
    }
    return szMani.LogicNode.createInputSocketsVector(builder, socketOffs);
  }
}
