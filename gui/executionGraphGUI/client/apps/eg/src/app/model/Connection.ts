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

import { OutputSocket, InputSocket } from "./Socket"

/**
 * Modelclass for a connection
 * @todo see -> SocketLink.fbs
 * @export
 * @class Connection
 */
export class Connection {
  private readonly _idString: string;

  constructor(
    public outputSocket: OutputSocket,
    public inputSocket: InputSocket,
    isWriteLink: boolean = true) {
    this._idString = `${this.outputSocket.idString}-to-${this.inputSocket.idString}`;
  }

  /**
   * Gets the string identifier for this connection.
   *
   * @readonly
   * @memberof Connection
   */
  public get idString(): string {
    return this._idString;
  }
}

export function createConnection(source: OutputSocket | InputSocket, target: OutputSocket | InputSocket) {
  if (source instanceof OutputSocket && target instanceof InputSocket) {
    // Make a Write-Link
    return new Connection(source, target, true);
  }
  else if (source instanceof InputSocket && target instanceof OutputSocket) {
    // Make a Get-Link
    return new Connection(target, source, false);
  }
  else {
    throw "Connection not allowed: Output <-> Input!";
  }
}
