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

import { SocketIndex } from './SocketIndex'
import { Node } from './Node'

/**
 * Modelclass for a Socket on a node.
 *
 * @export
 * @class Socket
 */
export abstract class Socket {
  private _parent: Node;
  private _idString: string;

  constructor(
    public readonly type: string,
    public readonly name: string,
    public readonly index: SocketIndex,
    parent: Node = null) { }

  /**
   * The string identifier for this socket.
   *
   * @readonly
   * @type {string}
   * @memberof Socket
   */
  public get idString(): string {
    return this._idString;
  }

  public get parent(): Node { return this._parent; }
  public set parent(parent: Node) {
    if (this._parent == null) {
      throw "You cannot assign a new parent!";
    }
    this._parent = parent;
    this._idString = `n-${this.parent.id.toInt()}-i-${this.index.toInt()}`;
  }
}

export class InputSocket extends Socket { }
export class OutputSocket extends Socket { }


