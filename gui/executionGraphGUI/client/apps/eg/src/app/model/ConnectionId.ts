// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Sep 15 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { SocketId } from "./SocketId"

/**
 * A class for defining a unique id for a `Connection`.
 *
 * @export
 * @class ConnectionId
 */
export class ConnectionId {
  private readonly _idString: string
  constructor(
    public readonly outSocketId: SocketId,
    public readonly inSocketId: SocketId,
    public readonly isWriteLink: boolean) {
    this._idString = `${outSocketId.string}-to-${inSocketId.string}`;
  }

  public equal(id: ConnectionId) {
    return this.outSocketId === id.outSocketId &&
      this.inSocketId === id.inSocketId &&
      this.isWriteLink === id.isWriteLink;
  }

  public get string() { return this._idString; }

}
