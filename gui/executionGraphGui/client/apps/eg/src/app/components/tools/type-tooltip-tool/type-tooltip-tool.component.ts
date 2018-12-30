import { Component, OnInit, Input } from '@angular/core';
import { Store } from '@ngrx/store';
import { Point, ConnectionDrawStyle, GraphComponent } from '@eg/graph';
import { ILogger, LoggerFactory } from '@eg/logger';

import { ToolComponent } from '../tool-component';
import { OutputSocket, InputSocket, Connection, Socket, SocketIndex } from '../../../model';
import { GraphsState } from '../../../+state/reducers';
import { AddConnection } from '../../../+state/actions';
import { resetComponentState } from '@angular/core/src/render3/instructions';

@Component({
  selector: 'eg-type-tooltip-tool',
  templateUrl: './type-tooltip-tool.component.html',
  styleUrls: ['./type-tooltip-tool.component.scss']
})
export class SocketTypeToolTipToolComponent extends ToolComponent implements OnInit {
  private readonly logger: ILogger;

  private toolTipMessages: string[] = [];
  private toolTipPosition: Point = { x: 0, y: 0 };

  constructor(private graph: GraphComponent, private store: Store<GraphsState>, loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('SocketTypeToolTipToolComponent');
  }

  ngOnInit() {
    this.socketEvents.onEnter.subscribe(e => {
      this.toolTipMessages.push(`Type: [${e.element.type}]`);
      this.toolTipPosition = this.graph.convertMouseToGraphPosition(e.mousePosition);
    });
    this.socketEvents.onLeave.subscribe(e => {
      this.reset();
    });

    this.nodeEvents.onEnter.subscribe(e => {
      this.toolTipMessages.push(`Type: [${e.element.type}]`);
      this.toolTipPosition = this.graph.convertMouseToGraphPosition(e.mousePosition);
    });
    this.nodeEvents.onLeave.subscribe(e => {
      this.reset();
    });
  }

  private reset() {
    this.toolTipMessages = [];
    this.toolTipPosition = { x: 0, y: 0 };
  }
}
