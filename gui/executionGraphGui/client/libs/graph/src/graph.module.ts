import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { GraphComponent } from '@eg/graph/src/components/graph/graph.component';
import { PortComponent } from '@eg/graph/src/components/port/port.component';
import { ConnectionComponent } from '@eg/graph/src/components/connection/connection.component';
import { DraggableDirective } from '@eg/graph/src/directives/draggable.directive';

@NgModule({
  imports: [CommonModule],
  declarations: [GraphComponent, PortComponent, ConnectionComponent, DraggableDirective],
  exports: [GraphComponent, PortComponent, ConnectionComponent, DraggableDirective]
})
export class GraphModule {}
