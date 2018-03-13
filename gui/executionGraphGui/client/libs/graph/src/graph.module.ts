import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { GraphComponent } from './components/graph/graph.component';
import { PortComponent } from './components/port/port.component';
import { ConnectionComponent } from './components/connection/connection.component';
import { DraggableDirective } from './directives/draggable.directive';

@NgModule({
  imports: [CommonModule],
  declarations: [GraphComponent, PortComponent, ConnectionComponent, DraggableDirective],
  exports: [GraphComponent, PortComponent, ConnectionComponent, DraggableDirective]
})
export class GraphModule {}
