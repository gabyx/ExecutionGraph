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

import { ConnectionDrawStyle } from '@eg/graph/src/model/ConnectionDrawStyle';
import { Point } from '@eg/graph/src/model/Point';

export class ManhattenConnectionDrawStyle extends ConnectionDrawStyle {
  getPath(start: Point, end: Point): Point[] {
    const rangeX = end.x - start.x;
    const rangeY = end.y - start.y;
    if (Math.abs(rangeX) > Math.abs(rangeY)) {
      const midX = start.x + rangeX / 2;
      return [start, { x: midX, y: start.y }, { x: midX, y: end.y }, end];
    } else {
      const midY = start.y + rangeY / 2;
      return [start, { x: start.x, y: midY }, { x: end.x, y: midY }, end];
    }
  }
}
