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

import { flatbuffers } from 'flatbuffers';
import * as model from './../model';
import * as serialization from '@eg/serialization';
import { createSocket } from '../model/Socket';
import { NodeMap, ConnectionMap } from '../model/Graph';
import { assert } from '@eg/common';
import { createConnection } from '../model/Connection';

export function toFbLong(value: number | string): flatbuffers.Long {
  if (typeof value === 'number') {
    return flatbuffers.Long.create(value, 0);
  } else {
    return flatbuffers.Long.create(parseInt(value, 10), 0);
  }
}

/**
 * Convert a `serialized`-instance of a graph type description to a `model`-instance.
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
 */
export function toNode(node: serialization.LogicNode): model.Node {
  // Convert to a node model
  const nodeId = node
    .id()
    .toFloat64()
    .toString();

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
      const socket = createSocket(kind, s.typeIndex().toFloat64(), s.index().toFloat64(), nodeId, s.typeName());
      sockets.push(socket);
    }
  };

  extractSockets('input', allSockets);
  extractSockets('output', allSockets);

  return model.fromNode.createNode(nodeId, node.type(), allSockets);
}

/**
 *  Convert a `serialized`-instance of a connection to a `model`-instance.
 */
export function toConnection(nodes: NodeMap, link: serialization.SocketLinkDescription): model.Connection {
  const outNodeId = link
    .outNodeId()
    .toFloat64()
    .toString();
  const inNodeId = link
    .inNodeId()
    .toFloat64()
    .toString();

  const outSocketIdx = link.outSocketIdx().toFloat64();
  const inSocketIdx = link.inSocketIdx().toFloat64();

  assert(inNodeId in nodes && outNodeId in nodes, 'In/Out node index not in map!');
  const inNode = nodes[inNodeId];
  const outNode = nodes[outNodeId];

  assert(outSocketIdx < outNode.outputs.length && inSocketIdx < inNode.inputs.length, 'Sockets indices wrong!');
  const outSocket = outNode.outputs[outSocketIdx];
  const inSocket = inNode.inputs[inSocketIdx];

  return link.isWriteLink() ? createConnection(outSocket, inSocket) : createConnection(inSocket, outSocket);
}

/**
 *  Convert a `serialized`-instance of a graph to a `model`-instance.
 */
export function toGraph(graphId: string, graph: serialization.ExecutionGraph): model.Graph {
  const nodes: NodeMap = {};
  let l = graph.nodesLength();
  for (let i = 0; i < l; ++i) {
    const node = toNode(graph.nodes(i));
    nodes[node.id.toString()] = node;
  }

  const connections: ConnectionMap = {};
  l = graph.linksLength();
  for (let i = 0; i < l; ++i) {
    const con = toConnection(nodes, graph.links(i));
    connections[con.id.toString()] = con;
  }

  return {
    id: graphId,
    name: 'Unnamed',
    connections: connections,
    nodes: nodes,
    typeId: graph.graphDescription().id()
  };
}
