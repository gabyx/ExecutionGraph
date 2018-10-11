import { Point } from "@eg/graph";

export interface GraphMouseEvent {
    /**
     * Mouse position in screen coordinates
     *
     * @type {Point}
     * @memberof GraphMouseEvent
     */
    screenPosition: Point;

    /**
     * Mouse position in graph coordinates
     *
     * @type {Point}
     * @memberof GraphMouseEvent
     */
    graphPosition: Point;
}

export abstract class ToolComponent {


    public activate() {}

    public deactivate() {}
}