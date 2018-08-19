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

import { InputSocket, OutputSocket } from "./Socket"

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
    this._idString =
      `n-${this.outputSocket.parent.id.toInt()}-o-${this.outputSocket.index.toInt()}` +
      `-to-` +
      `n-${this.inputSocket.parent.id.toInt()}-o-${this.inputSocket.index.toInt()}`;
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
