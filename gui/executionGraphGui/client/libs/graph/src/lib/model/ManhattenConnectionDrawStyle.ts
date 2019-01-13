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

import { ConnectionDrawStyle } from './ConnectionDrawStyle';
import { Point } from './Point';

export class ManhattenConnectionDrawStyle extends ConnectionDrawStyle {
  getPath(start: Point, end: Point): Point[] {
    const rangeX = end.x - start.x;
    const rangeY = end.y - start.y;
    if (Math.abs(rangeX) > Math.abs(rangeY)) {
      const midX = start.x + rangeX / 2;
      return [start, new Point(midX, start.y), new Point(midX, end.y), end];
    } else {
      const midY = start.y + rangeY / 2;
      return [start, new Point(start.x, midY), new Point(end.x, midY), end];
    }
  }
}
