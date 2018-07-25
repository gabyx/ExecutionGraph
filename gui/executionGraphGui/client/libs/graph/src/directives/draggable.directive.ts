import { Directive, ElementRef, HostListener, EventEmitter, Output } from '@angular/core';
import { Point } from '../model/Point';
import 'rxjs/add/operator/do';
import 'rxjs/add/operator/map';
import 'rxjs/add/operator/switchMap';
import 'rxjs/add/operator/takeUntil';

@Directive({
  selector: '[ngcsDraggable]'
})
export class DraggableDirective {
  
  @Output() dragged = new EventEmitter<Point>();

  private readonly mousePressed = new EventEmitter<MouseEvent>();
  private readonly mouseMoved = new EventEmitter<MouseEvent>();
  private readonly mouseReleased = new EventEmitter<MouseEvent>();
  
  private get nativeElement(): HTMLElement {
    return this.element.nativeElement;
  }

  constructor(private element: ElementRef) {
    this.mousePressed
      .map(event => ({
        startDragPoint: {
          x: event.clientX,// - this.nativeElement.getBoundingClientRect().left,
          y: event.clientY,// - this.nativeElement.getBoundingClientRect().top
        },
        startPosition: {
          x: (event.target as HTMLElement).offsetLeft,
          y: (event.target as HTMLElement).offsetTop
        }
      }))
      .switchMap(start => this.mouseMoved
        .do(e => {
          console.log(this.nativeElement.getBoundingClientRect());
          // console.log(this.nativeElement.cl);
        })
        .map(moveEvent => ({
          x: moveEvent.clientX,
          y: moveEvent.clientY
        }))
        .map(movePoint => ({
          x: start.startPosition.x + (movePoint.x - start.startDragPoint.x), 
          y: start.startPosition.y + (movePoint.y - start.startDragPoint.y)
        }))
      //.do(p => console.log(`Started at ${startPoint.x}:${startPoint.y}, Now at ${p.x}:${p.y}`))
      .do(point => {
        this.nativeElement.style.left = `${point.x}px`;
        this.nativeElement.style.top  = `${point.y}px`;
      })
      .do(point => this.dragged.emit(point))
      .takeUntil(this.mouseReleased))
      .subscribe(() => void 0);
  }

  @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {
    event.preventDefault();
    this.mousePressed.emit(event);
  }

  @HostListener('document:mouseup', ['$event']) onMouseUp(event: MouseEvent) {
    event.preventDefault();
    this.mouseReleased.emit(event);
  }

  @HostListener('document:mousemove', ['$event']) onMouseMove(event: MouseEvent) {
    event.preventDefault();
    this.mouseMoved.emit(event);
  }


}
