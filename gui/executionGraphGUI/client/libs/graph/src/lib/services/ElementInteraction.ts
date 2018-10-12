import { Point } from "../model/Point";
import { Observable, Subject, Subscription } from "rxjs";

export enum MouseButton {
    Left = 0,
    Right = 2
}

export interface ElementMouseEvent<TElement> {
    mousePosition: Point;
    elementOffset: Point;
    element: TElement;
}

export interface ElementMouseButtonEvent<TElement> extends ElementMouseEvent<TElement> {
    button: MouseButton;
}

export interface ElementDragEvent<TElement> {
    started: ElementMouseButtonEvent<TElement>;
    current: ElementMouseButtonEvent<TElement>;
}

export interface IElementEvents<TElement> {
    readonly onEnter: Observable<ElementMouseEvent<TElement>>;
    readonly onLeave: Observable<ElementMouseEvent<TElement>>;
    readonly onMove: Observable<ElementMouseEvent<TElement>>;

    readonly onDown: Observable<ElementMouseButtonEvent<TElement>>;
    readonly onUp: Observable<ElementMouseButtonEvent<TElement>>;
    readonly onClick: Observable<ElementMouseButtonEvent<TElement>>;

    readonly onDragStart: Observable<ElementMouseButtonEvent<TElement>>;
    readonly onDragContinue: Observable<ElementMouseButtonEvent<TElement>>;
    readonly onDragStop: Observable<ElementMouseButtonEvent<TElement>>;
}

interface GatewaySourceSubscription {
    readonly source: any,
    readonly onEnter: Subscription;
    readonly onLeave: Subscription;
    readonly onMove: Subscription;
    readonly onDown: Subscription;
    readonly onUp: Subscription;
    readonly onClick: Subscription;
    readonly onDragStart: Subscription;
    readonly onDragContinue: Subscription;
    readonly onDragStop: Subscription;
}

export class EventSourceGateway<TElement> implements IElementEvents<TElement> {

    private readonly _onEnter = new Subject<ElementMouseEvent<TElement>>();
    private readonly _onLeave = new Subject<ElementMouseEvent<TElement>>();
    private readonly _onMove = new Subject<ElementMouseEvent<TElement>>();

    private readonly _onDown = new Subject<ElementMouseButtonEvent<TElement>>();
    private readonly _onUp = new Subject<ElementMouseButtonEvent<TElement>>();
    private readonly _onClick = new Subject<ElementMouseButtonEvent<TElement>>();

    private readonly _onDragStart = new Subject<ElementMouseButtonEvent<TElement>>();
    private readonly _onDragContinue = new Subject<ElementMouseButtonEvent<TElement>>();
    private readonly _onDragStop = new Subject<ElementMouseButtonEvent<TElement>>();

    private subscriptions: GatewaySourceSubscription[] = [];

    public get onEnter(): Observable<ElementMouseEvent<TElement>> {
        return this._onEnter.asObservable();
    }
    public get onLeave(): Observable<ElementMouseEvent<TElement>> {
        return this._onLeave.asObservable();
    }
    public get onMove(): Observable<ElementMouseEvent<TElement>> {
        return this._onMove.asObservable();
    }

    public get onDown(): Observable<ElementMouseButtonEvent<TElement>> {
        return this._onDown.asObservable();
    }
    public get onUp(): Observable<ElementMouseButtonEvent<TElement>> {
        return this._onUp.asObservable();
    }
    public get onClick(): Observable<ElementMouseButtonEvent<TElement>> {
        return this._onClick.asObservable();
    }

    public get onDragStart(): Observable<ElementMouseButtonEvent<TElement>> {
        return this._onDragStart.asObservable();
    }
    public get onDragContinue(): Observable<ElementMouseButtonEvent<TElement>> {
        return this._onDragContinue.asObservable();
    }
    public get onDragStop(): Observable<ElementMouseButtonEvent<TElement>> {
        return this._onDragStop.asObservable();
    }

    public forward(source: IElementEvents<TElement>) {
        const subscription = {
            source: source,

            onEnter: source.onEnter.subscribe(e => this._onEnter.next(e)),
            onLeave: source.onLeave.subscribe(e => this._onLeave.next(e)),
            onMove: source.onMove.subscribe(e => this._onMove.next(e)),
            onDown: source.onDown.subscribe(e => this._onDown.next(e)),
            onUp: source.onUp.subscribe(e => this._onUp.next(e)),
            onClick: source.onClick.subscribe(e => this._onClick.next(e)),
            onDragStart: source.onDragStart.subscribe(e => this._onDragStart.next(e)),
            onDragContinue: source.onDragContinue.subscribe(e => this._onDragContinue.next(e)),
            onDragStop: source.onDragStop.subscribe(e => this._onDragStop.next(e)),
        };
        this.subscriptions.push(subscription);
    }

    public mute(source: IElementEvents<TElement>) {
        this.subscriptions.splice(this.subscriptions.findIndex(s => s.source===source), 1);
    }
}