import { Component, OnInit } from '@angular/core';
import { Point, MouseButton, GraphComponent } from '@eg/graph';

import { ToolComponent } from './tool-component';

@Component({
    selector: 'eg-navigation-tool',
    template: ''
})
export class NavigationToolComponent extends ToolComponent implements OnInit {

    constructor(private graph: GraphComponent) {
      super();
    }

    ngOnInit() {
        const panStart: Point = {x: 0, y: 0};
        this.graphEvents.onDragStart.subscribe((e) => {
            if(e.button === MouseButton.Right) {
                const mousePosition = this.graph.convertMouseToGraphPosition(e.mousePosition);
                panStart.x = mousePosition.x;
                panStart.y = mousePosition.y;
            }
        });

        this.graphEvents.onDragContinue.subscribe((e) => {
            if (e.button === MouseButton.Right) {
                const position = this.graph.convertMouseToGraphPosition(e.mousePosition);
                this.graph.pan.x += position.x - panStart.x;
                this.graph.pan.y += position.y - panStart.y;
            }
        });

        this.graphEvents.onScroll.subscribe(e => {
          const zoomSpeed = 0.98;
          if (e.delta < 0) {
            this.graph.zoomFactor *= zoomSpeed;
            // this.graph.pan.x *= zoomSpeed;
            // this.graph.pan.y *= zoomSpeed;
          } else {
            this.graph.zoomFactor /= zoomSpeed;
            // this.graph.pan.x /= zoomSpeed;
            // this.graph.pan.y /= zoomSpeed;
          }
        });

        // this.graphEvents.onDragStop.subscribe(e => console.log("Graph drag stop"));
        // this.graphEvents.onMove.subscribe(e => console.log("Graph move"));
        // this.graphEvents.onDown.subscribe(e => console.log("Graph down"));

        // this.graphEvents.onDown.subscribe(e => console.log("Graph down"));
        // this.graphEvents.onEnter.subscribe(e => console.log("Graph enter"));
        // this.graphEvents.onLeave.subscribe(e => console.log("Graph leave"));

    }

}
