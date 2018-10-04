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

import { OutputSocket, InputSocket, SocketId, isInputSocket, isOutputSocket } from './Socket';

/**
 * A class for defining a unique id for a `Connection`.
 *
 * @export
 * @class ConnectionId
 */
export class ConnectionId {
  private readonly _idString: string;
  constructor(public outSocketId: SocketId, public inSocketId: SocketId, public isWriteLink: boolean) {
    this._idString = `${outSocketId.string}-to-${inSocketId.string}`;
  }

  public equal(id: ConnectionId) {
    return this.string == id.string;
  }

  public get string() {
    return this._idString;
  }
}

/**
 * Modelclass for a connection
 * @todo see -> SocketLink.fbs
 * @export
 * @class Connection
 */
export class Connection {
  public readonly id: ConnectionId;

  constructor(public outputSocket: OutputSocket, public inputSocket: InputSocket, isWriteLink: boolean = true) {
    this.id = new ConnectionId(outputSocket.id, inputSocket.id, isWriteLink);
  }

  /**
   * Gets the string identifier for this connection.
   *
   * @readonly
   * @memberof Connection
   */
  public get idString(): string {
    return this.id.string;
  }
}

export function createConnection(source: OutputSocket | InputSocket, target: OutputSocket | InputSocket) {
  if (isOutputSocket(source) && isInputSocket(target)) {
    // Make a Write-Link
    return new Connection(source, target, true);
  } else if (isInputSocket(source) && isOutputSocket(target)) {
    // Make a Get-Link
    return new Connection(target, source, false);
  } else {
    throw new Error('Connection not allowed: Output <-> Input!');
  }
}
