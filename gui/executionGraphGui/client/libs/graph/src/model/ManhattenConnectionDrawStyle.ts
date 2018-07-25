import { ConnectionDrawStyle } from "./ConnectionDrawStyle";
import { Point } from "./Point";

export class ManhattenConnectionDrawStyle extends ConnectionDrawStyle {
    getPath(start: Point, end: Point): Point[] {
        const rangeX = end.x - start.x;
        const rangeY = end.y - start.y;
        if (Math.abs(rangeX) > Math.abs(rangeY)) {
            const midX = start.x + rangeX / 2;
            return [start, { x: midX, y: start.y }, { x: midX, y: end.y }, end];
        }
        else {
            const midY = start.y + rangeY / 2;
            return [start, { x: start.x, y: midY }, { x: end.x, y: midY }, end];
        }
    }
}
