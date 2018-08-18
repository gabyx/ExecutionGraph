import { Directive, EventEmitter, Output, ElementRef, Renderer2, Input } from '@angular/core';
import { DraggableDirective } from '@eg/graph/src/directives/draggable.directive';

@Directive({
  selector: '[ngcsDroppable]'
})
export class DroppableDirective {
  @Output() draggableEntered = new EventEmitter<any>();
  @Output() draggableLeft = new EventEmitter<any>();
  @Output() draggableDropped = new EventEmitter<any>();

  @Input('ngcsDroppable') dropAcceptanceFilter: (data: any) => boolean;
  @Input() droppingClass: string = 'dropping';
  @Input() dropAllowedClass: string = 'dropping-allowed';
  @Input() dropForbiddenClass: string = 'dropping-forbidden';

  private draggable: DraggableDirective = null;
  private isActive = false;
  private isAccepted = false;

  private onMouseEnterRegistration: any;
  private onMouseLeaveRegistration: any;

  constructor(public element: ElementRef, private renderer: Renderer2) {
    this.onMouseEnterRegistration = this.onMouseEnter.bind(this);
    this.onMouseLeaveRegistration = this.onMouseLeave.bind(this);
  }

  startTracking(draggable: DraggableDirective) {
    this.draggable = draggable;
    this.element.nativeElement.addEventListener('mouseenter', this.onMouseEnterRegistration);
    this.element.nativeElement.addEventListener('mouseleave', this.onMouseLeaveRegistration);
  }

  stopTracking() {
    this.element.nativeElement.removeEventListener('mouseenter', this.onMouseEnterRegistration);
    this.element.nativeElement.removeEventListener('mouseleave', this.onMouseLeaveRegistration);
    this.clearClasses();

    if (this.isActive) {
      if (this.isAccepted) {
        this.draggableDropped.emit(this.draggable.data);
      }
      this.isActive = false;
    }
    this.draggable = null;
  }

  onMouseEnter(event: MouseEvent) {
    this.isActive = true;
    this.renderer.addClass(this.element.nativeElement, this.droppingClass);
    if (this.dropAcceptanceFilter) {
      this.isAccepted = this.dropAcceptanceFilter(this.draggable.data);
      this.renderer.addClass(
        this.element.nativeElement,
        this.isAccepted ? this.dropAllowedClass : this.dropForbiddenClass
      );
    }
    this.draggableEntered.emit(this.draggable.data);
  }

  onMouseLeave(event: MouseEvent) {
    this.clearClasses();

    this.isActive = false;
    this.draggableLeft.emit(this.draggable.data);
  }

  private clearClasses() {
    this.renderer.removeClass(this.element.nativeElement, this.droppingClass);
    this.renderer.removeClass(this.element.nativeElement, this.dropAllowedClass);
    this.renderer.removeClass(this.element.nativeElement, this.dropForbiddenClass);
  }
}
