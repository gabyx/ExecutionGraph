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

export interface UIProps {
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
  constructor(
    public readonly id: NodeId,
    public readonly type: string,
    public readonly name: string,
    public inputs: InputSocket[] = [],
    public outputs: OutputSocket[] = [],
    public uiProps: UIProps = { x: 0, y: 0 }
  ) {
    // Sorting input/outputs according to index.
    const sort = (a: Socket, b: Socket) => a.index.comp(b.index);
    this.inputs = inputs.sort(sort);
    this.outputs = outputs.sort(sort);
    // Setting all parents!
    this.inputs.forEach((s: Socket) => (s.parent = this));
    this.outputs.forEach((s: Socket) => (s.parent = this));
  }

  public get idString() : string {
    return this.id.string;
  }
}
