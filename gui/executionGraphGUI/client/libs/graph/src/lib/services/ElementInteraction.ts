import { Point } from "../model/Point";
import { Observable, Subject, Subscription, BehaviorSubject, merge } from "rxjs";
import { switchMap } from "rxjs/operators";

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

export interface ElementMouseScrollEvent<TElement> extends ElementMouseEvent<TElement> {
    delta: number
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

    readonly onScroll: Observable<ElementMouseScrollEvent<TElement>>;
}

export class EventSourceGateway<TElement> implements IElementEvents<TElement> {

    private readonly lastSources: IElementEvents<TElement>[] = [];
    private readonly sources = new BehaviorSubject<IElementEvents<TElement>[]>(this.lastSources);

    public get onEnter(): Observable<ElementMouseEvent<TElement>> {
        return this.sources.pipe(
          switchMap(sources => merge(...sources.map(s => s.onEnter)))
        );
    }
    public get onLeave(): Observable<ElementMouseEvent<TElement>> {
      return this.sources.pipe(
        switchMap(sources => merge(...sources.map(s => s.onLeave)))
      );
    }
    public get onMove(): Observable<ElementMouseEvent<TElement>> {
        return this.sources.pipe(
            switchMap(sources => merge(...sources.map(s => s.onMove)))
        );
    }

    public get onDown(): Observable<ElementMouseButtonEvent<TElement>> {
        return this.sources.pipe(
            switchMap(sources => merge(...sources.map(s => s.onDown)))
        );
    }
    public get onUp(): Observable<ElementMouseButtonEvent<TElement>> {
        return this.sources.pipe(
            switchMap(sources => merge(...sources.map(s => s.onUp)))
        );
    }
    public get onClick(): Observable<ElementMouseButtonEvent<TElement>> {
        return this.sources.pipe(
            switchMap(sources => merge(...sources.map(s => s.onClick)))
        );
    }

    public get onDragStart(): Observable<ElementMouseButtonEvent<TElement>> {
        return this.sources.pipe(
            switchMap(sources => merge(...sources.map(s => s.onDragStart)))
        );
    }
    public get onDragContinue(): Observable<ElementMouseButtonEvent<TElement>> {
        return this.sources.pipe(
            switchMap(sources => merge(...sources.map(s => s.onDragContinue)))
        );
    }
    public get onDragStop(): Observable<ElementMouseButtonEvent<TElement>> {
        return this.sources.pipe(
            switchMap(sources => merge(...sources.map(s => s.onDragStop)))
        );
    }

    public get onScroll(): Observable<ElementMouseScrollEvent<TElement>> {
        return this.sources.pipe(
            switchMap(sources => merge(...sources.map(s => s.onScroll)))
        );
    }

    public forward(source: IElementEvents<TElement>) {
        this.lastSources.push(source);
        this.sources.next(this.lastSources);
    }

    public mute(source: IElementEvents<TElement>) {
        this.lastSources.splice(this.lastSources.findIndex(s => s===source), 1);
        this.sources.next(this.lastSources);
    }
}
