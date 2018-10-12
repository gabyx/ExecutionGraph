import { Directive, Input, HostListener, EventEmitter, ElementRef, OnDestroy } from "@angular/core";
import { IElementEvents, ElementMouseEvent, ElementMouseButtonEvent, EventSourceGateway } from "../services/ElementInteraction";
import { Position } from "../model/Point";
import { Observable } from "rxjs";
import { switchMap, map, takeUntil } from "rxjs/operators";

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
    @Input('ngcsEventSource') element: TElement;

    @Input('forwardTo') set forwardTo(gateway: EventSourceGateway<TElement>) {
        if(this.gateway !== null) {
            this.gateway.mute(this);
        }
        this.gateway = gateway;

        if(this.gateway !== null) {
            this.gateway.forward(this);
        }
    }

    private readonly mouseEntered = new EventEmitter<ElementMouseEvent<TElement>>();
    private readonly mouseLeft = new EventEmitter<ElementMouseEvent<TElement>>();
    private readonly mousePressed = new EventEmitter<ElementMouseButtonEvent<TElement>>();
    private readonly mouseReleased = new EventEmitter<ElementMouseButtonEvent<TElement>>();
    private readonly mouseMoved = new EventEmitter<ElementMouseEvent<TElement>>();

    private gateway: EventSourceGateway<TElement> = null;

    constructor(private elementRef: ElementRef) {
    }

    ngOnDestroy() {
        if(this.gateway!==null) {
            this.gateway.mute(this);
        }
    }

    public get onDown(): Observable<ElementMouseButtonEvent<TElement>> {
        return this.mousePressed.asObservable();
    }

    public get onUp(): Observable<ElementMouseButtonEvent<TElement>> {
        return this.mouseReleased.asObservable();
    }

    public get onClick(): Observable<ElementMouseButtonEvent<TElement>> {
        return this.mousePressed
            .pipe(switchMap(down => this.mousePressed.asObservable()));
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
        return this.onDragStart.pipe(
            switchMap(down => this.onMove.pipe(
                map(move => ({...move, button: down.button}))
            ))
        );
    }

    public get onDragStop(): Observable<ElementMouseButtonEvent<TElement>> {
        return this.onDragContinue.pipe(
            takeUntil(this.onUp)
        );
    }

    private get nativeElement(): HTMLElement {
        return this.elementRef.nativeElement;
    }

    private get elementPosition(): Position {
        const clientRect = this.nativeElement.getBoundingClientRect();
        return {
            x: clientRect.left,
            y: clientRect.top
        };
    }

    @HostListener('mouseenter', ['$event']) private onMouseEnter(event: MouseEvent) {
        this.mouseEntered.emit(this.convertEvent(event));
    }

    @HostListener('mouseleave', ['$event']) private onMouseLeave(event: MouseEvent) {
        this.mouseLeft.emit(this.convertEvent(event));
    }

    @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {
        this.mousePressed.emit(this.convertButtonEvent(event));
    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {
        this.mouseMoved.emit(this.convertEvent(event));
    }

    @HostListener('mouseup', ['$event']) onMouseUp(event: MouseEvent) {
        this.mouseReleased.emit(this.convertButtonEvent(event));
    }

    private convertEvent(mouseEvent: MouseEvent): ElementMouseEvent<TElement> {
        const elementPosition = this.elementPosition;

        return {
            mousePosition: {
                x: mouseEvent.clientX,
                y: mouseEvent.clientY
            },
            elementOffset: {
                x: mouseEvent.clientX - elementPosition.x,
                y: mouseEvent.clientY - elementPosition.y
            },
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
}