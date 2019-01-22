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

import { Point } from '@eg/graph';
import * as Long from 'long';
import { Socket, OutputSocket, InputSocket, isOutputSocket } from './Socket';

export interface UIProps {
  readonly name: string;
  position: Point;
}
function createUIProps(name: string = 'Unnamed', position: Point = Point.zero.copy()) {
  return { name: name, position: position };
}

export type NodeId = Long;
export type NodeType = string;

export interface Node {
  readonly id: NodeId;
  readonly type: NodeType;
  readonly inputs: InputSocket[];
  readonly outputs: OutputSocket[];
  readonly uiProps: UIProps;
}

/*
 * Create a node.
 */
export function createNode(id: NodeId, type: NodeType, sockets?: Socket[], uiProps?: UIProps): Node {
  const inputs: InputSocket[] = [];
  const outputs: OutputSocket[] = [];

  if (sockets) {
    sockets.forEach((s: InputSocket | OutputSocket) => {
      if (isOutputSocket(s)) {
        outputs.push(s);
      } else {
        inputs.push(s);
      }
    });
  }

  const sort = (a: Socket, b: Socket) => a.index.comp(b.index);
  inputs.sort(sort);
  outputs.sort(sort);

  return {
    id: id,
    type: type,
    inputs: inputs,
    outputs: outputs,
    uiProps: uiProps ? uiProps : createUIProps()
  };
}
