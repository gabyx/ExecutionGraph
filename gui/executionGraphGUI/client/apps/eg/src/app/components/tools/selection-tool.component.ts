import { Component, OnInit, HostListener } from '@angular/core';
import { Store } from '@ngrx/store';
import { Observable } from 'rxjs';
import { filter, tap, withLatestFrom } from 'rxjs/operators';

import { Point, MouseButton } from '@eg/graph';

import { ToolComponent } from './tool-component';
import { UiState, Selection } from '../../+state/reducers/ui.reducers';
import * as fromUiSelectors from '../../+state/selectors/ui.selectors';
import * as fromUiActions from '../../+state/actions/ui.actions';

enum KEY_CODE {
    BACKSPACE = 8,
    SHIFT = 16,
    CTRL = 17,
    DELETE = 46,
}

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

    private readonly selection: Observable<Selection>;

    private isExtending: boolean = false;

    constructor(private store: Store<UiState>) {
        super();
        this.selection = store.select(fromUiSelectors.getSelection);
    }

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

        this.nodeEvents.onClick.pipe( withLatestFrom(this.selection) )
            .subscribe(([e, selection]) => {
                if(this.isExtending) {
                    if(selection.nodes.indexOf(e.element.id) < 0) {
                        this.store.dispatch(new fromUiActions.AddSelection([e.element.id]));
                    } else {
                        this.store.dispatch(new fromUiActions.RemoveSelection([e.element.id]));
                    }
                } else {
                    this.store.dispatch(new fromUiActions.SetSelection([e.element.id], [...selection.connections]));
                }
            });
        this.graphEvents.onClick.subscribe(e => this.store.dispatch(new fromUiActions.ClearSelection()));
    }

    @HostListener('window:keydown', ['$event'])
    onKeyDown(keyEvent: KeyboardEvent) {
        if (keyEvent.keyCode === KEY_CODE.CTRL || keyEvent.keyCode === KEY_CODE.SHIFT) {
            this.isExtending = true;
        }
    }

    @HostListener('window:keyup', ['$event'])
    onKeyUp(keyEvent: KeyboardEvent) {
        if (keyEvent.keyCode === KEY_CODE.CTRL || keyEvent.keyCode === KEY_CODE.SHIFT) {
            this.isExtending = false;
        }
    }


}
