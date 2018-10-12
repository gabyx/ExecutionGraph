import { Component, OnInit } from '@angular/core';
import { Point } from '@eg/graph';

import { ToolComponent } from './tool-component';

@Component({
    selector: 'eg-navigation-tool',
    template: ''
})
export class NavigationToolComponent extends ToolComponent implements OnInit {


    ngOnInit() {
        const panStart: Point = {x: 0, y: 0};
        this.graphEvents.onDragStart.subscribe((e) => {
            const mousePosition = e.element.convertMouseToGraphPosition(e.mousePosition);
            panStart.x = mousePosition.x;
            panStart.y = mousePosition.y;
        });

        this.graphEvents.onDragContinue.subscribe((e) => {
            const position = e.element.convertMouseToGraphPosition(e.mousePosition);
            e.element.pan.x += position.x - panStart.x;
            e.element.pan.y += position.y - panStart.y;

            console.log(`[WorkspaceComponent] Panned to ${e.element.pan.x}:${e.element.pan.y}`);
        });

        // this.graphEvents.onDragStop.subscribe(e => console.log("Graph drag stop"));
        // this.graphEvents.onMove.subscribe(e => console.log("Graph move"));
        // this.graphEvents.onDown.subscribe(e => console.log("Graph down"));

        // this.graphEvents.onDown.subscribe(e => console.log("Graph down"));
        // this.graphEvents.onEnter.subscribe(e => console.log("Graph enter"));
        // this.graphEvents.onLeave.subscribe(e => console.log("Graph leave"));

    }

}