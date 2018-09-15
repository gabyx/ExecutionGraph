// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Tue Aug 14 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import * as Long from 'long';


function isLong(value: any): value is Long {
  return value instanceof Long;
}

export class NodeId extends Long {
  private readonly _idString: string
  constructor(id: number | Long) {
    if (isLong(id)) {
      super(id.low, id.high, id.unsigned);
    }
    else {
      super(id, 0, true);
    }
    this._idString = `n-${this.toInt()}`
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

}
