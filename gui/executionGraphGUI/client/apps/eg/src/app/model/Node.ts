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

import { Socket, InputSocket, OutputSocket } from './Socket';
import { NodeId } from './NodeId';
import { Input } from '@angular/compiler/src/core';

export type UIProps = {
  x: number;
  y: number;
};

/**
 *  Modelclass for a node.
 *
 * @export
 * @class Node
 */
export class Node {
  private readonly _idString: string;

  constructor(
    public readonly id: NodeId,
    public readonly type: string,
    public readonly name: string,
    public inputs: InputSocket[] = [],
    public outputs: OutputSocket[] = [],
    public uiProps: UIProps = { x: 0, y: 0 }
  ) {
    // Sorting input/outputs according to index.
    let s = (a: Socket, b: Socket) => a.index.comp(b.index);
    this.inputs = inputs.sort(s);
    this.outputs = outputs.sort(s);
    // Setting all parents!
    this.inputs.forEach((s: Socket) => (s.parent = this));
    this.outputs.forEach((s: Socket) => (s.parent = this));
    this._idString = `n-${this.id.toInt()}`;
  }

  /**
   * String identifer for this node.
   *
   * @returns {string}
   * @memberof Node
   */
  public get idString(): string {
    return this._idString;
  }
}
