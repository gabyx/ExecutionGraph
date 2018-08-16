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

export class BezierConnectionDrawStyle extends ConnectionDrawStyle {
  constructor(public curviness: number = 1) {
    super();
  }

  getPath(start: Point, end: Point): string {
    if (!start || !end) {
      return ``;
    }
    const rangeX = end.x - start.x;

    const points = [
      start,
      { x: start.x + rangeX * this.curviness, y: start.y },
      { x: end.x - this.curviness * rangeX, y: end.y },
      end
    ];

    const pointStrings = points.map(p => `${p.x},${p.y}`);
    pointStrings[0] = `M${pointStrings[0]}`;
    pointStrings[1] = `C${pointStrings[1]}`;
    return pointStrings.join(' ');
  }
}
