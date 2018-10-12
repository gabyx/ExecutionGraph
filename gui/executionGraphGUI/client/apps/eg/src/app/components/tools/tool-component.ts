import { Point, GraphComponent, IElementEvents } from "@eg/graph";
import { Input } from "@angular/core";

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

    @Input() graph: GraphComponent;

    @Input() nodeEvents: IElementEvents<Node>;
}