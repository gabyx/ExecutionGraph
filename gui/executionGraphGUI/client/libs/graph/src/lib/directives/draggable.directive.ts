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

import { Directive, ElementRef, HostListener, EventEmitter, Output, Input } from '@angular/core';

import { tap, map, switchMap, takeUntil } from 'rxjs/operators';

import { Point } from '../model/Point';

export type DragMouseEvent = {
  elementPosition: Point;
  mousePosition: Point;
};

export type DragEvent = {
  dragElementPosition: Point;
  mousePosition: Point;
  mouseToElementOffset: Point;
};

@Directive({
  selector: '[ngcsDraggable]'
})
export class DraggableDirective {
  @Input('ngcsDraggable') data: any = null;

  @Output() dragStarted = new EventEmitter<DragEvent>();
  @Output() dragContinued = new EventEmitter<DragEvent>();
  @Output() dragEnded = new EventEmitter<DragEvent>();

  @Input()
  set dragElement(value: HTMLElement) {
    this._nativeElement = value;
    this._nativeElement['draggableElement'] = this;
  }

  private readonly mousePressed = new EventEmitter<DragMouseEvent>();
  private readonly mouseMoved = new EventEmitter<DragMouseEvent>();
  private readonly mouseReleased = new EventEmitter<DragMouseEvent>();

  private _nativeElement: HTMLElement;

  public get nativeElement(): HTMLElement {
    return this._nativeElement;
  }

  constructor(private element: ElementRef) {
    //Hack for faster access to the component from events
    this.dragElement = element.nativeElement;

    this.mousePressed
      .pipe(
        tap(dragStartEvent =>
          console.log(
            `Started at Element: ${dragStartEvent.elementPosition.x}:${dragStartEvent.elementPosition.y}, Mouse: ${
              dragStartEvent.mousePosition.x
            }:${dragStartEvent.mousePosition.y}`
          )
        ),
        tap(dragStartEvent => this.dragStarted.emit(this.calculateDragEvent(dragStartEvent, dragStartEvent))),
        switchMap(dragStartEvent =>
          this.mouseMoved.pipe(
            map(dragMoveEvent => this.calculateDragEvent(dragStartEvent, dragMoveEvent)),
            // .do(e => console.log(`Now at ${e.dragElementPosition.x}:${e.dragElementPosition.y}`))
            tap(point => {
              // this.nativeElement.style.left = `${point.x}px`;
              // this.nativeElement.style.top  = `${point.y}px`;
            }),
            tap(point => this.dragContinued.emit(point)),
            takeUntil(
              this.mouseReleased.pipe(
                map(dragEndEvent => this.calculateDragEvent(dragStartEvent, dragEndEvent)),
                // .do(p => console.log(`Ended at ${p.x}:${p.y}`))
                tap(p => this.dragEnded.emit(p))
              )
            )
          )
        )
      )
      .subscribe(() => void 0);
  }

  onMouseDown(event: DragMouseEvent) {
    this.mousePressed.emit(event);
  }

  onMouseUp(event: DragMouseEvent) {
    this.mouseReleased.emit(event);
  }

  onMouseMove(event: DragMouseEvent) {
    this.mouseMoved.emit(event);
  }

  private calculateDragEvent(startEvent: DragMouseEvent, currentEvent: DragMouseEvent): DragEvent {
    const clientRect = this.nativeElement.getBoundingClientRect();
    let scale = clientRect.width / this.nativeElement.offsetWidth;
    const result = {
      dragElementPosition: {
        x: startEvent.elementPosition.x + (currentEvent.mousePosition.x - startEvent.mousePosition.x) / scale,
        y: startEvent.elementPosition.y + (currentEvent.mousePosition.y - startEvent.mousePosition.y) / scale
      },
      mouseToElementOffset: {
        x: (startEvent.mousePosition.x - clientRect.left) / scale,
        y: (startEvent.mousePosition.y - clientRect.top) / scale
      },
      mousePosition: currentEvent.mousePosition
    };
    return result;
  }
}
