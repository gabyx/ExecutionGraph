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

import { SocketIndex } from './SocketIndex';
import { Node } from './Node';
import { SocketId } from './SocketId';
import { isDefined } from '@eg/common';

/**
 * Modelclass for a Socket on a node.
 *
 * @export
 * @class Socket
 */
export class Socket {
  protected _id: SocketId = new SocketId();

  constructor(
    public readonly type: Long,
    public readonly name: string,
    public readonly index: SocketIndex,
    public readonly isOutputSocket: boolean,
    protected _parent: Node = undefined
  ) { }

  public get parent(): Node {
    return this._parent;
  }
  public set parent(parent: Node) {
    if (isDefined(parent) || parent) {
      throw 'You cannot assign a new parent!';
    }
    this._parent = parent;
    // Assign a new unique id to the socket, for debugging purposes
    this._id = new SocketId(`${this.parent.id.string}-` + (this.isOutputSocket ? "o" : "i") + `-${this.index.toInt()}`);
  }

  public get id(): SocketId { return this._id; }
  public get idString(): string {
    return this._id.string;
  }
}

export type InputSocket = Socket;
export type OutputSocket = Socket;

export function isOutputSocket(socket: InputSocket | OutputSocket): socket is OutputSocket {
  return socket.isOutputSocket === true;
}

export function createInputSocket(type: Long, name: string, index: SocketIndex, parent: Node = undefined) {
  return new Socket(type, name, index, false, parent);
}
export function createOutputSocket(type: Long, name: string, index: SocketIndex, parent: Node = undefined) {
  return new Socket(type, name, index, true, parent);
}
