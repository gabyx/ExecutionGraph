import { Component, OnInit, Inject, OnDestroy, ElementRef } from '@angular/core';
import { GraphComponent } from '../graph/graph.component';

@Component({
  selector: 'ngcs-port',
  templateUrl: './port.component.html',
  styleUrls: ['./port.component.scss']
})
export class PortComponent implements OnInit, OnDestroy {

  public get id(): string {
    return this.element.nativeElement.id;
  }
  
  constructor(@Inject(GraphComponent) private readonly graph: GraphComponent, private readonly element: ElementRef) {}

  ngOnInit() {
    this.graph.registerPort(this.id, this);
  }

  ngOnDestroy() {
    this.graph.registerPort(this.id, this);
  }

  connectTo(otherPort: PortComponent) {
    console.log(`[PortComponent] ${this.id} connected to ${otherPort.id}`);
  }
}
