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
import { Socket, InputSocket, OutputSocket } from './Socket';
import * as fromSocket from './Socket';

function isLong(value: any): value is Long {
  return value instanceof Long;
}

export class UIProps {
  public name: string = 'Unnamed';
  public position = Point.zero.copy();
}

/**
 * Unique Identifier for a `Node`.
 *
 */
export class NodeId extends Long {
  constructor(id: number | Long) {
    if (isLong(id)) {
      super(id.low, id.high, id.unsigned);
    } else {
      super(id, 0, true);
    }
  }
}

/**
 *  Modelclass for a node.
 */
export class Node {
  /** Two different lists for sockets */
  public readonly inputs: InputSocket[] = [];
  public readonly outputs: OutputSocket[] = [];
  constructor(
    public readonly id: NodeId,
    public readonly type: string,
    public sockets: Socket[] = [],
    public uiProps: UIProps = new UIProps()
  ) {
    // Make
    sockets.forEach((s: InputSocket | OutputSocket) => {
      if (fromSocket.isOutputSocket(s)) {
        this.outputs.push(s);
      } else {
        this.inputs.push(s);
      }
    });
    // Sorting input/outputs according to index.
    const sort = (a: Socket, b: Socket) => a.index.comp(b.index);
    this.inputs = this.inputs.sort(sort);
    this.outputs = this.outputs.sort(sort);
    // Setting all parents!
    const setter = (s: Socket) => {
      s.parent = this;
      s.id = fromSocket.createId(this, s.index, s.kind);
    };
    this.inputs.forEach(setter);
    this.outputs.forEach(setter);
  }
}
