import { GraphComponent, IElementEvents } from "@eg/graph";
import { Input } from "@angular/core";
import { Socket, Node } from "../../model";

export abstract class ToolComponent {

    @Input() graph: GraphComponent;

    @Input() nodeEvents: IElementEvents<Node>;

    @Input() graphEvents: IElementEvents<GraphComponent>;

    @Input() socketEvents: IElementEvents<Socket>;
}
