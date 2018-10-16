import { GraphComponent, IElementEvents } from "@eg/graph";
import { Input } from "@angular/core";
import { Socket, Node } from "../../model";

export enum KEY_CODE {
    BACKSPACE = 8,
    SHIFT = 16,
    CTRL = 17,
    DELETE = 46,
    ESC = 27,
    A = 65
}

export abstract class ToolComponent {

    @Input() graph: GraphComponent;

    @Input() nodeEvents: IElementEvents<Node>;

    @Input() graphEvents: IElementEvents<GraphComponent>;

    @Input() socketEvents: IElementEvents<Socket>;
}
