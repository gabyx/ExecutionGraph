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
import { Id } from '@eg/common';
import * as model from './../model';
import * as serialization from '@eg/serialization';

export function toLong(value: flatbuffers.Long): Long {
  return Long.fromBits(value.low, value.high, false);
}

export function toULong(value: flatbuffers.Long): Long {
  return Long.fromBits(value.low, value.high, true);
}

/**
 * Convert a `serialized`-instance of a graph type description to a `model`-instance.
 *
 * @export
 * @param {serialization.GraphTypeDescription} graphDesc
 * @returns {model.GraphTypeDescription}
 */
export function toGraphTypeDescription(graphDesc: serialization.GraphTypeDescription): model.GraphTypeDescription {
  let sockets: model.SocketTypeDescription[] = [];
  for (let i = 0; i < graphDesc.socketTypeDescriptionsLength(); ++i) {
    let s = graphDesc.socketTypeDescriptions(i);
    sockets.push({ type: s.type(), name: s.name() });
  }

  let nodes: model.NodeTypeDescription[] = [];
  for (let i = 0; i < graphDesc.nodeTypeDescriptionsLength(); ++i) {
    let s = graphDesc.nodeTypeDescriptions(i);
    nodes.push({ type: s.type(), name: s.name() });
  }

  return {
    id: new Id(graphDesc.id()),
    name: graphDesc.name(),
    nodeTypeDescritptions: nodes,
    socketTypeDescriptions: sockets
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
  let nodeId = new model.NodeId(toULong(node.id()));

  let sockets: model.Socket[] = [];

  // Convert the sockets
  let extractSockets = (kind: model.SocketType, sockets: model.Socket[]) => {
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
      let s = socks(i);
      let socket = model.createSocket(kind, toULong(s.type()), s.name(), toULong(s.index()));
      sockets.push(socket);
    }
  };

  extractSockets('input', sockets);
  extractSockets('output', sockets);

  return new model.Node(nodeId, node.type(), node.name(), sockets);
}
