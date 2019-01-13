import { Component, OnInit } from '@angular/core';
import { Point, MouseButton, GraphComponent } from '@eg/graph';

import { ToolComponent } from '../tool-component';

@Component({
  selector: 'eg-navigation-tool',
  template: ''
})
export class NavigationToolComponent extends ToolComponent implements OnInit {
  constructor(private graph: GraphComponent) {
    super();
  }

  ngOnInit() {
    const panStart: Point = Point.one.copy();
    this.graphEvents.onDragStart.subscribe(e => {
      if (e.button === MouseButton.Right) {
        const mousePosition = this.graph.convertMouseToGraphPosition(e.mousePosition);
        panStart.x = mousePosition.x;
        panStart.y = mousePosition.y;
      }
    });

    this.graphEvents.onDragContinue.subscribe(e => {
      if (e.button === MouseButton.Right) {
        const position = this.graph.convertMouseToGraphPosition(e.mousePosition);
        this.graph.pan.x += position.x - panStart.x;
        this.graph.pan.y += position.y - panStart.y;
      }
    });

    this.graphEvents.onScroll.subscribe(e => {
      const zoomSpeed = 0.95;
      const graphPositionBefore = this.graph.convertMouseToGraphPosition(e.mousePosition);
      if (e.deltaY < 0) {
        this.graph.zoomFactor *= zoomSpeed;
      } else {
        this.graph.zoomFactor /= zoomSpeed;
      }
      const graphPositionAfter = this.graph.convertMouseToGraphPosition(e.mousePosition);

      this.graph.pan.x += graphPositionAfter.x - graphPositionBefore.x;
      this.graph.pan.y += graphPositionAfter.y - graphPositionBefore.y;
    });
  }
}
