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
  selector: 'eg-socket-type-tooltip-tool',
  templateUrl: './socket-type-tooltip-tool.component.html',
  styleUrls: ['./socket-type-tooltip-tool.component.scss']
})
export class SocketTypeToolTipToolComponent extends ToolComponent implements OnInit {
  @Input()
  private readonly logger: ILogger;

  private toolTipSocket: Socket | null = null;
  private toolTipPosition: Point = { x: 0, y: 0 };

  constructor(private graph: GraphComponent, private store: Store<GraphsState>, loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('SocketTypeToolTipToolComponent');
  }

  ngOnInit() {
    this.socketEvents.onEnter.subscribe(e => {
      this.toolTipSocket = e.element;
      this.toolTipPosition = this.graph.convertMouseToGraphPosition(e.mousePosition);
    });
    this.socketEvents.onMove.subscribe(e => {
      this.toolTipPosition = this.graph.convertMouseToGraphPosition(e.mousePosition);
    });
    this.socketEvents.onLeave.subscribe(e => {
      this.reset();
    });
  }

  private reset() {
    this.toolTipSocket = null;
    this.toolTipPosition = { x: 0, y: 0 };
  }
}
