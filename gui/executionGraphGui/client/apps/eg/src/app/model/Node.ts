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

function isLong(value: any): value is Long {
  return value instanceof Long;
}

export class UIProps {
  public position = Point.zero.copy();
}

/**
 * Unique Identifier for a `Node`.
 *
 * @export
 * @class NodeId
 * @extends {Long}
 */
export class NodeId extends Long {
  private readonly _idString: string;
  constructor(id: number | Long) {
    if (isLong(id)) {
      super(id.low, id.high, id.unsigned);
    } else {
      super(id, 0, true);
    }
    this._idString = `n-${this.toInt()}`;
  }
  /**
   * String identifer for this NodeId.
   *
   * @returns {string}
   * @memberof NodeId
   */
  public get string(): string {
    return this._idString;
  }
}

/**
 *  Modelclass for a node.
 *
 * @export
 * @class Node
 */
export class Node {
  /** Two different lists for sockets */
  public readonly inputs: InputSocket[] = [];
  public readonly outputs: OutputSocket[] = [];
  constructor(
    public readonly id: NodeId,
    public readonly type: string,
    public readonly name: string,
    public sockets: Socket[] = [],
    public uiProps: UIProps = new UIProps()
  ) {
    // Make
    sockets.forEach((s: InputSocket | OutputSocket) => {
      if (Socket.isOutputSocket(s)) {
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
    this.inputs.forEach((s: Socket) => (s.parent = this));
    this.outputs.forEach((s: Socket) => (s.parent = this));
  }

  public get idString(): string {
    return this.id.string;
  }
}
