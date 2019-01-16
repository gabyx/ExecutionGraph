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

import { vec2 } from '@tlaukkan/tsm';

export class Point extends vec2 {
  constructor(x: number, y: number) {
    super();
    this.x = x;
    this.y = y;
  }
}

export type Position = Point;
export type Vector2 = Point;
