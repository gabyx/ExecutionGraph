import { Directive, Input, HostListener, EventEmitter, ElementRef, OnDestroy } from '@angular/core';
import {
  IElementEvents,
  ElementMouseEvent,
  ElementMouseButtonEvent,
  EventSourceGateway,
  ElementMouseScrollEvent
} from '../services/ElementInteraction';
import { Position, Point } from '../model/Point';
import { isArray } from '@eg/common';
import { Observable, fromEvent } from 'rxjs';
import { switchMap, map, takeUntil, tap, first, mergeMap, concatMap } from 'rxjs/operators';

@Directive({
  selector: '[ngcsEventSource]'
})
export class ElementEventSourceDirective<TElement> implements IElementEvents<TElement>, OnDestroy {
  /**
   * The element that will be passed on with the events
   *
   * @type {TElement}
   * @memberof ElementEventSourceDirective
   */
  @Input('ngcsEventSource')
  element: TElement;

  @Input('forwardTo')
  set forwardTo(gateway: EventSourceGateway<TElement> | EventSourceGateway<TElement>[]) {
    const gateways = isArray(gateway) ? [...gateway] : [gateway];

    if (this.gateway !== null) {
      this.gateway.forEach(g => g.mute(this));
    }
    this.gateway = gateways;

    if (this.gateway !== null) {
      this.gateway.forEach(g => g.forward(this));
    }
  }

  @Input('cancelBubble')
  set cancelBubble(enable: boolean) {
    this.cancelBubble_ = enable;
  }

  private readonly mouseEntered = new EventEmitter<ElementMouseEvent<TElement>>();
  private readonly mouseLeft = new EventEmitter<ElementMouseEvent<TElement>>();
  private readonly mousePressed = new EventEmitter<ElementMouseButtonEvent<TElement>>();
  private readonly mouseReleased = new EventEmitter<ElementMouseButtonEvent<TElement>>();
  private readonly mouseMoved = new EventEmitter<ElementMouseEvent<TElement>>();
  private readonly mouseScrolled = new EventEmitter<ElementMouseScrollEvent<TElement>>();
  private readonly documentMouseMoved = new EventEmitter<ElementMouseEvent<TElement>>();
  private readonly documentMouseReleased = new EventEmitter<ElementMouseButtonEvent<TElement>>();

  private gateway: EventSourceGateway<TElement>[] = null;
  private cancelBubble_: boolean = true;

  constructor(private elementRef: ElementRef) {}

  ngOnDestroy() {
    if (this.gateway !== null) {
      this.gateway.forEach(g => g.mute(this));
    }
  }

  public get onDown(): Observable<ElementMouseButtonEvent<TElement>> {
    return this.mousePressed.asObservable();
  }

  public get onUp(): Observable<ElementMouseButtonEvent<TElement>> {
    return this.mouseReleased.asObservable();
  }

  public get onClick(): Observable<ElementMouseButtonEvent<TElement>> {
    return this.mousePressed.pipe(
      switchMap(down =>
        this.mouseReleased.pipe(
          first(),
          takeUntil(this.mouseMoved)
        )
      )
    );
  }

  public get onEnter(): Observable<ElementMouseEvent<TElement>> {
    return this.mouseEntered.asObservable();
  }

  public get onLeave(): Observable<ElementMouseEvent<TElement>> {
    return this.mouseLeft.asObservable();
  }

  public get onMove(): Observable<ElementMouseEvent<TElement>> {
    return this.mouseMoved.asObservable();
  }

  public get onDragStart(): Observable<ElementMouseButtonEvent<TElement>> {
    return this.mousePressed.asObservable();
  }

  public get onDragContinue(): Observable<ElementMouseButtonEvent<TElement>> {
    return this.mousePressed.pipe(
      mergeMap(down =>
        this.documentMouseMoved.pipe(
          map(move => ({ ...move, button: down.button })),
          takeUntil(this.documentMouseReleased)
        )
      )
    );
  }

  public get onDragStop(): Observable<ElementMouseButtonEvent<TElement>> {
    return this.onDragContinue.pipe(switchMap(drag => this.documentMouseReleased));
  }

  public get onScroll(): Observable<ElementMouseScrollEvent<TElement>> {
    return this.mouseScrolled.asObservable();
  }

  private get nativeElement(): HTMLElement {
    return this.elementRef.nativeElement;
  }

  private get elementPosition(): Position {
    const clientRect = this.nativeElement.getBoundingClientRect();
    return new Point(clientRect.left, clientRect.top);
  }

  @HostListener('mouseenter', ['$event'])
  onMouseEnter(event: MouseEvent) {
    this.mouseEntered.emit(this.convertEvent(event));
  }

  @HostListener('mouseleave', ['$event'])
  onMouseLeave(event: MouseEvent) {
    this.mouseLeft.emit(this.convertEvent(event));
  }

  @HostListener('mousedown', ['$event'])
  onMouseDown(event: MouseEvent) {
    event.cancelBubble = this.cancelBubble_;
    event.preventDefault();
    this.mousePressed.emit(this.convertButtonEvent(event));
  }

  @HostListener('mousemove', ['$event'])
  onMouseMove(event: MouseEvent) {
    // Fix Chrome Bug
    if (event.movementX === 0 && event.movementY === 0) {
      return;
    }
    event.preventDefault();
    this.mouseMoved.emit(this.convertEvent(event));
  }

  @HostListener('document:mousemove', ['$event'])
  onDocumentMouseMove(event: MouseEvent) {
    this.documentMouseMoved.emit(this.convertEvent(event));
  }

  @HostListener('mouseup', ['$event'])
  onMouseUp(event: MouseEvent) {
    //   event.cancelBubble = true;
    this.mouseReleased.emit(this.convertButtonEvent(event));
  }

  @HostListener('document:mouseup', ['$event'])
  onDocumentMouseUp(event: MouseEvent) {
    this.documentMouseReleased.emit(this.convertButtonEvent(event));
  }

  @HostListener('wheel', ['$event'])
  onMouseScroll(e: WheelEvent) {
    e.preventDefault();
    this.mouseScrolled.emit(this.convertScrollEvent(e));
  }

  private convertEvent(mouseEvent: MouseEvent): ElementMouseEvent<TElement> {
    const elementPosition = this.elementPosition;

    return {
      mousePosition: new Point(mouseEvent.clientX, mouseEvent.clientY),
      elementOffset: new Point(mouseEvent.clientX - elementPosition.x, mouseEvent.clientY - elementPosition.y),
      element: this.element
    };
  }

  private convertButtonEvent(mouseEvent: MouseEvent): ElementMouseButtonEvent<TElement> {
    const event = this.convertEvent(mouseEvent);
    return {
      ...event,
      button: mouseEvent.button
    };
  }

  private convertScrollEvent(wheelEvent: WheelEvent): ElementMouseScrollEvent<TElement> {
    const event = this.convertEvent(wheelEvent);
    return {
      ...event,
      deltaX: wheelEvent.deltaX,
      deltaY: wheelEvent.deltaY
    };
  }
}
