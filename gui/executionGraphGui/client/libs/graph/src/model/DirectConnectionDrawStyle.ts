import { ConnectionDrawStyle } from "./ConnectionDrawStyle";
import { Point } from "./Point";

export class DirectConnectionDrawStyle extends ConnectionDrawStyle {
  getPath(start: Point, end: Point): Point[] {
    return [start, end];
  }
}
