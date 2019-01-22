// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Aug 19 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import * as Long from 'long';
import { flatbuffers } from 'flatbuffers';
import * as model from './../model';
import * as serialization from '@eg/serialization';
import { createSocket } from '../model/Socket';

export function toLong(value: flatbuffers.Long): Long {
  return Long.fromBits(value.low, value.high, false);
}

export function toULong(value: flatbuffers.Long): Long {
  return Long.fromBits(value.low, value.high, true);
}

export function toFbLong(value: Long): flatbuffers.Long {
  return flatbuffers.Long.create(value.low, value.high);
}

/**
 * Convert a `serialized`-instance of a graph type description to a `model`-instance.
 *
 * @export
 * @param {serialization.GraphTypeDescription} graphDesc
 * @returns {model.GraphTypeDescription}
 */
export function toGraphTypeDescription(graphDesc: serialization.GraphTypeDescription): model.GraphTypeDescription {
  const sockets: model.SocketTypeDescription[] = [];
  for (let i = 0; i < graphDesc.socketTypeDescriptionsLength(); ++i) {
    const s = graphDesc.socketTypeDescriptions(i);

    const d = s.description();
    sockets.push({ type: s.type(), name: s.name(), description: d ? d : undefined });
  }

  const nodes: model.NodeTypeDescription[] = [];
  for (let i = 0; i < graphDesc.nodeTypeDescriptionsLength(); ++i) {
    const s = graphDesc.nodeTypeDescriptions(i);

    const inS: string[] = [];
    for (let j = 0; j < s.inSocketNamesLength(); ++j) {
      inS.push(s.inSocketNames(j));
    }

    const outS: string[] = [];
    for (let j = 0; j < s.outSocketNamesLength(); ++j) {
      outS.push(s.outSocketNames(j));
    }
    const d = s.description();

    nodes.push({
      type: s.type(),
      name: s.name(),
      inSocketNames: inS,
      outSocketNames: outS,
      description: d ? d : undefined
    });
  }

  return {
    id: graphDesc.id(),
    name: graphDesc.name(),
    nodeTypeDescriptions: nodes,
    socketTypeDescriptions: sockets,
    description: graphDesc.description()
  };
}

/**
 *  Convert a `serialized`-instance of a node to a `model`-instance.
 *
 * @export
 * @param {serialization.LogicNode} node
 * @returns {Node}
 */
export function toNode(node: serialization.LogicNode): model.Node {
  // Convert to a node model
  const nodeId = toULong(node.id());

  const allSockets: model.Socket[] = [];

  // Convert the sockets
  const extractSockets = (kind: model.SocketType, sockets: model.Socket[]) => {
    let l: number;
    let socks: (idx: number) => serialization.LogicSocket;
    if (kind === 'output') {
      l = node.outputSocketsLength();
      socks = (idx: number) => node.outputSockets(idx);
    } else {
      l = node.inputSocketsLength();
      socks = (idx: number) => node.inputSockets(idx);
    }

    for (let i = 0; i < l; ++i) {
      const s = socks(i);
      const socket = createSocket(kind, toULong(s.typeIndex()), toULong(s.index()), nodeId, s.typeName());
      sockets.push(socket);
    }
  };

  extractSockets('input', allSockets);
  extractSockets('output', allSockets);

  return model.fromNode.createNode(nodeId, node.type(), allSockets);
}
