import { Component } from '@angular/core';
import { Point } from '@eg/graph';

import { Tool, GraphMouseEvent } from './Tool';

@Component({
    template: `
    <rect
        *ngIf="isSelecting"
        [attr.x]="selectionAreaX"
        [attr.y]="selectionAreaY"
        [attr.width]="selectionAreaWidth"
        [attr.height]="selectionAreaHeight"
        style="fill:blue;stroke:pink;stroke-width:5;fill-opacity:0.1;stroke-opacity:0.9" />
    `
})
export class SelectionToolComponent extends Tool {

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


    public onDragStart(dragEvent: GraphMouseEvent)  {
        this.dragStartPoint = dragEvent.graphPosition;
    }

    public onDrag(dragPoint: GraphMouseEvent) {
        this.dragPoint = dragPoint.graphPosition;
    }

    public onDragEnd(dragEvent: GraphMouseEvent) {
        this.dragStartPoint = null;
        this.dragPoint = null;
    }
}