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

import * as Long from 'long';
import { Guid } from 'guid-typescript';
import { Node } from './Node';
import { isDefined } from '@eg/common';
import { Input } from '@angular/compiler/src/core';

export class UIProps {
  public name: string = '';
}

/**
 * The socket index of a socket `Socket`.
 *
 * @export
 * @class SocketIndex
 * @extends {Long}
 */
export class SocketIndex extends Long {}

/**
 * Unique Identifier for a `Socket`.
 *
 * @export
 * @class SocketId
 */
export class SocketId {
  constructor(private readonly _idString: string = Guid.create().toString()) {}
  /**
   * String identifer for this NodeId.
   *
   * @returns {string}
   * @memberof NodeId
   */
  public get string(): string {
    return this._idString;
  }

  public equal(id: SocketId) {
    return this._idString === id._idString;
  }
}

/**
 * Modelclass for a Socket on a node.
 *
 * @export
 * @class Socket
 */
export abstract class Socket {
  abstract get kind(): SocketType;
  protected _id: SocketId = new SocketId();

  constructor(
    public readonly typeIndex: Long,
    public readonly index: SocketIndex,
    public readonly type?: string,
    protected _parent?: Node,
    public uiProps: UIProps = new UIProps()
  ) {
    if (!isDefined(type)) {
      type = typeIndex.toString();
    }
  }

  public get parent(): Node {
    return this._parent;
  }
  public set parent(parent: Node) {
    if (isDefined(this.parent) || !isDefined(parent)) {
      throw new Error('You cannot assign a new parent or undefined!');
    }
    this._parent = parent;
    // Assign a new unique id to the socket, for debugging purposes
    this._id = this.createId();
  }
  public get id(): SocketId {
    return this._id;
  }
  public get idString(): string {
    return this._id.string;
  }
  protected abstract createId(): SocketId;

  /** Type guard for InputSocket */
  public static isInputSocket(socket: Socket): socket is InputSocket {
    return socket.kind === 'input';
  }

  /** Type guard for OutputSocket */
  public static isOutputSocket(socket: Socket): socket is OutputSocket {
    return socket.kind === 'output';
  }

  /**
   * Creator function for input/output sockets.
   *
   * @export
   * @param {SocketType} kind
   * @param {Long} type
   * @param {string} typeName
   * @param {string} name
   * @param {SocketIndex} index
   * @param {Node} [parent=undefined]
   * @returns
   */
  public static createSocket(
    kind: SocketType,
    type: Long,
    index: SocketIndex,
    typeName: string | undefined | null,
    parent?: Node
  ): InputSocket | OutputSocket {
    switch (kind) {
      case 'input': {
        const s = new InputSocket(type, index, isDefined(typeName) ? typeName : undefined, parent);
        // @todo gabnue->gabnue: Move that to
        // the designated place where we initialize UI-Properties.
        if (!s.uiProps.name) {
          s.uiProps.name = `in-${s.index}`;
        }
        return s;
      }
      case 'output': {
        const s = new OutputSocket(type, index, isDefined(typeName) ? typeName : undefined, parent);
        if (!s.uiProps.name) {
          s.uiProps.name = `out-${s.index}`;
        }
        return s;
      }
    }
  }
}

export type SocketType = 'input' | 'output';

/**
 * The output socket.
 *
 * @export
 * @class InputSocket
 * @extends {Socket}
 */
export class InputSocket extends Socket {
  public readonly kind: SocketType = 'input';
  protected createId(): SocketId {
    return new SocketId(`${this.parent.id.string}-i-${this.index.toInt()}`);
  }
}

/**
 * The input socket.
 *
 * @export
 * @class OutputSocket
 * @extends {Socket}
 */
export class OutputSocket extends Socket {
  public readonly kind: SocketType = 'output';
  protected createId(): SocketId {
    return new SocketId(`${this.parent.id.string}-o-${this.index.toInt()}`);
  }
}
