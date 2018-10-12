import { Component, OnInit } from '@angular/core';
import { Point } from '@eg/graph';

import { ToolComponent, GraphMouseEvent } from './tool-component';

@Component({
    selector: 'eg-selection-tool',
    template: `
    <svg:rect class="selection-preview"
        *ngIf="isSelecting"
        [attr.x]="selectionAreaX"
        [attr.y]="selectionAreaY"
        [attr.width]="selectionAreaWidth"
        [attr.height]="selectionAreaHeight"
        ></svg:rect>
    `,
    styles: [`
    .selection-preview {
        stroke: pink;
        stroke-width: 1;
        stroke-opacity: 0.9;
        fill: black;
        fill-opacity: 0.1;
    }
    `]
})
export class SelectionToolComponent extends ToolComponent implements OnInit {

    private dragStartPoint: Point = null;

    private dragPoint: Point = null;

    public get isSelecting() {
        return this.dragStartPoint !== null && this.dragPoint !== null;
    }

    public get selectionAreaX() {
        return Math.min(this.dragStartPoint.x, this.dragPoint.x);
    }

    public get selectionAreaY() {
        return Math.min(this.dragStartPoint.y, this.dragPoint.y);
    }

    public get selectionAreaWidth() {
        return Math.abs(this.dragStartPoint.x - this.dragPoint.x);
    }

    public get selectionAreaHeight() {
        return Math.abs(this.dragStartPoint.y - this.dragPoint.y);
    }

    ngOnInit() {
        this.nodeEvents.onMove.subscribe(() => console.log("mouse moved"));
        this.nodeEvents.onDown.subscribe(() => console.log("mouse down"));
    }

    // public onDragStart(dragEvent: GraphMouseEvent)  {
    //     this.dragStartPoint = dragEvent.graphPosition;
    // }

    // public onDrag(dragPoint: GraphMouseEvent) {
    //     this.dragPoint = dragPoint.graphPosition;
    // }

    // public onDragEnd(dragEvent: GraphMouseEvent) {
    //     this.dragStartPoint = null;
    //     this.dragPoint = null;
    // }
}