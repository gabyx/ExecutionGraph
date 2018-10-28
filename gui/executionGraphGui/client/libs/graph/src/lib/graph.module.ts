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

import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { GraphComponent } from './components/graph/graph.component';
import { PortComponent } from './components/port/port.component';
import { DraggableDirective } from './directives/draggable.directive';
import { DroppableDirective } from './directives/droppable.directive';
import { DragAndDropService } from '@eg/graph/src/lib/services/drag-and-drop.service';
import { ElementEventSourceDirective } from './directives/element-event-source.directive';
import { SvgLayerComponent } from './components/svg-layer/svg-layer.component';
import { HtmlLayerComponent } from './components/html-layer/html-layer.component';

@NgModule({
  imports: [CommonModule],
  declarations: [GraphComponent, PortComponent, DraggableDirective, DroppableDirective, ElementEventSourceDirective, SvgLayerComponent, HtmlLayerComponent],
  exports: [GraphComponent, PortComponent, DraggableDirective, DroppableDirective, ElementEventSourceDirective, SvgLayerComponent, HtmlLayerComponent],
  providers: [DragAndDropService]
})
export class GraphModule {}
