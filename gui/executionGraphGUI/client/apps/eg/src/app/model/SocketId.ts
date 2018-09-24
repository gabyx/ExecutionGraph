// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Aug 19 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { SocketIndex } from "./SocketIndex"
import { NodeId } from "./NodeId"
import { Guid } from 'guid-typescript';

/**
 * Unique Identifier for a socket `Socket`.
 *
 * @export
 * @class SocketId
 */
export class SocketId {
  private readonly _idString: string
  constructor(
    private readonly parentId: NodeId = null,
    private readonly index: SocketIndex = SocketIndex.fromInt(0),
    private readonly outputSocket: boolean = true
  ) {
    if (parentId) {
      this._idString = `${parentId.string}-` + (outputSocket ? "o" : "i") + `-${index.toInt()}`
    }
    else {
      this._idString = Guid.create().toString();
    }
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

  public equal(id: SocketId) {
    return this.parentId === id.parentId &&
      this.index === id.index &&
      this.outputSocket === id.outputSocket
  }
}
