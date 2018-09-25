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

import { Guid } from 'guid-typescript';

/**
 * Unique Identifier for a socket `Socket`.
 *
 * @export
 * @class SocketId
 */
export class SocketId {
  constructor(private readonly _idString: string = Guid.create().toString()) {
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
    return this._idString === id._idString;
  }
}
