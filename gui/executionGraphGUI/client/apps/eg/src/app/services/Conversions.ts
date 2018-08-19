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

import * as Long from "long"
import { flatbuffers } from "flatbuffers"
import { Id } from '@eg/common'
import * as model from "./../model"
import * as serialization from '@serialization/index'

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
  let nodeId = toULong(node.id());

  let sockets: {
    input: model.Socket[];
    output: model.Socket[];
  } = { input: [], output: [] };

  for (let key of ["input", "output"]) {
    let isInput = key == "input";
    let l = node[`${key}SocketsLength`]();
    let arrayFunc = node[`${key}Sockets`];
    for (let i = 0; i < l; ++i) {
      let s = arrayFunc(i);
      sockets[key].push({
        nodeId: nodeId,
        type: s.type(),
        name: s.name(),
        index: toULong(s.index()),
        isInput: isInput
      });
    }
  }

  return new model.Node(
    nodeId,
    node.type(),
    node.name(),
    sockets.input,
    sockets.output);
}
