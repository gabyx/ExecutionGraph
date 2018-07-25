import { Point } from "./Point";

export abstract class ConnectionDrawStyle {

    public width: number = 1;

    public color: string = 'white';

    abstract getPath(start: Point, end: Point): string | Point[];
}