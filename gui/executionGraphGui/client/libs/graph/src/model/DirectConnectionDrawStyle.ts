import { ConnectionDrawStyle } from "@eg/graph/src/model/ConnectionDrawStyle";
import { Point } from "@eg/graph/src/model/Point";

export class DirectConnectionDrawStyle extends ConnectionDrawStyle {
  getPath(start: Point, end: Point): Point[] {
    return [start, end];
  }
}
