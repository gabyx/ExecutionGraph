import { Component, OnInit } from '@angular/core';
import { Point, MouseButton } from '@eg/graph';

import { ToolComponent } from './tool-component';
import { filter } from 'rxjs/operators';

@Component({
    selector: 'eg-selection-tool',
    template: `
    <ngcs-svg-layer *ngIf="isSelecting">

        <svg:rect class="selection-preview"

            [attr.x]="selectionAreaX"
            [attr.y]="selectionAreaY"
            [attr.width]="selectionAreaWidth"
            [attr.height]="selectionAreaHeight"
            ></svg:rect>

    </ngcs-svg-layer>
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
        this.graphEvents.onDragStart
            .pipe(filter(e => e.button === MouseButton.Left))
            .subscribe(e => this.dragStartPoint = this.graph.convertMouseToGraphPosition(e.mousePosition));
        this.graphEvents.onDragContinue
            .pipe(filter(e => e.button === MouseButton.Left))
            .subscribe(e => this.dragPoint = this.graph.convertMouseToGraphPosition(e.mousePosition));
        this.graphEvents.onDragStop
            .pipe(filter(e => e.button === MouseButton.Left))
            .subscribe(e => { this.dragPoint = null; this.dragStartPoint = null;});
    }
}
