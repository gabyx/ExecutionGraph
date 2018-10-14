import { Component, OnInit } from "@angular/core";
import { Store } from "@ngrx/store";

import { ToolComponent } from "./tool-component";
import { GraphsState } from "../../+state/reducers";
import { MoveNode } from "../../+state/actions";
import { Point } from "@eg/graph/src";

@Component({
  selector: 'eg-move-tool',
  template: ''
})
export class MoveToolComponent extends ToolComponent implements OnInit {

  constructor(private store: Store<GraphsState>) {
    super();
  }

  ngOnInit() {
    let elementOffset: Point;
    this.nodeEvents.onDragStart.subscribe(e => { elementOffset = e.elementOffset;});
    this.nodeEvents.onDragContinue.subscribe(dragEvent => {
        const nodePosition = {
          x: dragEvent.mousePosition.x - elementOffset.x,
          y: dragEvent.mousePosition.y - elementOffset.y
        };
        // console.log(`Mouse: ${dragEvent.mousePosition.x}:${dragEvent.mousePosition.y}, ElementOffset: ${elementOffset.x}:${elementOffset.y}`);
        const graphPosition = this.graph.convertMouseToGraphPosition(nodePosition);
        this.store.dispatch(new MoveNode(dragEvent.element, graphPosition));
    });
  }

}
