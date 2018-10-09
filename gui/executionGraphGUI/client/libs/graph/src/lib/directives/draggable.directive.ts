// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jul 29 2018
//  @author Simon Spoerri, simon (døt) spoerri (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { Directive, ElementRef, EventEmitter, Output, Input, HostListener, } from '@angular/core';

import { tap, map, switchMap, takeUntil } from 'rxjs/operators';

import { Point } from '../model/Point';
import { DragAndDropService } from '@eg/graph/src/lib/services/DragAndDropServices';

export enum MouseButton {
  Left = 0,
  Right = 2
}

export interface DragMouseEvent {
  mouseToElementOffset?: Point;
  mousePosition: Point;
  mouseButton: MouseButton;
};

export interface DragEvent {
  mousePosition: Point;
  mouseToElementOffset: Point;
  mouseButton: MouseButton;
};

@Directive({
  selector: '[ngcsDraggable]'
})
export class DraggableDirective {
  @Input('ngcsDraggable')
  data: any = null;

  @Output()
  dragStarted = new EventEmitter<DragEvent>();
  @Output()
  dragContinued = new EventEmitter<DragEvent>();
  @Output()
  dragEnded = new EventEmitter<DragEvent>();

  @Input()
  set dragElement(value: HTMLElement) {
    this._nativeElement = value;
  }

  private readonly mousePressed = new EventEmitter<DragMouseEvent>();
  private readonly mouseMoved = new EventEmitter<DragMouseEvent>();
  private readonly mouseReleased = new EventEmitter<DragMouseEvent>();

  private _nativeElement: HTMLElement;

  private isTracking: boolean = false;

  private readonly onMouseMoveRegistration = this.onMouseMove.bind(this);
  private readonly onMouseUpRegistration = this.onMouseUp.bind(this);

  public get nativeElement(): HTMLElement {
    return this._nativeElement;
  }

  constructor(element: ElementRef, private dragAndDropService: DragAndDropService) {
    // //Hack for faster access to the component from events
    this.dragElement = element.nativeElement;

    this.mousePressed
      .pipe(
        // tap(dragStartEvent =>
        //   console.log(
        //     `Started at Element: ${dragStartEvent.elementPosition.x}:${dragStartEvent.elementPosition.y}, Mouse: ${
        //       dragStartEvent.mousePosition.x
        //     }:${dragStartEvent.mousePosition.y}`
        //   )
          // ),
        tap(dragStartEvent => this.dragStarted.emit(this.calculateDragEvent(dragStartEvent, dragStartEvent))),
        switchMap(dragStartEvent =>
          this.mouseMoved.pipe(
            map(dragMoveEvent => this.calculateDragEvent(dragStartEvent, dragMoveEvent)),
            // tap(e => console.log(`Now at ${e.mousePosition.x}:${e.mousePosition.y} ${e.mouseToElementOffset.x}:${e.mousePosition.y}`)),
            tap(point => {
              // this.nativeElement.style.left = `${point.x}px`;
              // this.nativeElement.style.top  = `${point.y}px`;
            }),
            tap(point => this.dragContinued.emit(point)),
            takeUntil(
              this.mouseReleased.pipe(
                map(dragEndEvent => this.calculateDragEvent(dragStartEvent, dragEndEvent)),
                tap(dragEndEvent => this.dragAndDropService.stopTracking(dragEndEvent)),
                // tap(p => console.log(`Ended at ${p.dragElementPosition.x}:${p.dragElementPosition.y}`)),
                tap(p => this.dragEnded.emit(p))
              )
            )
          )
        )
      )
      .subscribe(() => void 0);

  }

  /**
   * Handles mouse button down events for drag starts
   * @param event
   */
  @HostListener('mousedown', ['$event'])
  onMouseDown(event: MouseEvent) {

    document.addEventListener('mouseup', this.onMouseUpRegistration);
    document.addEventListener('mousemove', this.onMouseMoveRegistration);

    event.preventDefault();
    event.cancelBubble = true;
    const clientRect = this.nativeElement.getBoundingClientRect();
    //const scale = clientRect.width / this.nativeElement.offsetWidth;

    const dragEvent = {
      mouseToElementOffset: {
        x: (event.clientX - clientRect.left),
        y: (event.clientY - clientRect.top)
      },
      mousePosition: { x: event.clientX, y: event.clientY },
      mouseButton: event.button
    };

    this.isTracking = true;

    this.dragAndDropService.startTracking(this);
    this.mousePressed.emit(dragEvent);
  }

  /**
   * Handles Mouse Button releases for drag ends
   * @param event Mouse Event
   */
  onMouseUp(event: MouseEvent) {

    if (this.isTracking) {
      this.isTracking = false;

      event.preventDefault();
      event.cancelBubble = true;
      const dragEvent = {
        mousePosition: { x: event.clientX, y: event.clientY },
        mouseButton: event.button
      };
      this.mouseReleased.emit(dragEvent);
    }

    document.removeEventListener('mouseup', this.onMouseUpRegistration);
    document.removeEventListener('mousemove', this.onMouseMoveRegistration);
  }

  /**
   * Handles mouse movement for dragging
   * @param event Mouse move event
   */
  onMouseMove(event: MouseEvent) {
    if (this.isTracking) {
      event.preventDefault();
      event.cancelBubble = true;

      const dragEvent = {
        mousePosition: { x: event.clientX, y: event.clientY },
        mouseButton: event.button
      };
      this.mouseMoved.emit(dragEvent);
    }
  }

  private calculateDragEvent(startEvent: DragMouseEvent, currentEvent: DragMouseEvent): DragEvent {
    const result = {
      mouseToElementOffset: startEvent.mouseToElementOffset,
      mousePosition: currentEvent.mousePosition,
      mouseButton: currentEvent.mouseButton
    };
    return result;
  }
}
