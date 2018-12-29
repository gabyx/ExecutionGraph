import { Component, OnInit, Input } from '@angular/core';
import { Store } from '@ngrx/store';
import { Point, ConnectionDrawStyle, GraphComponent } from '@eg/graph';
import { ILogger, LoggerFactory } from '@eg/logger';

import { ToolComponent } from '../tool-component';
import { OutputSocket, InputSocket, Connection, Socket, SocketIndex } from '../../../model';
import { GraphsState } from '../../../+state/reducers';
import { AddConnection } from '../../../+state/actions';

@Component({
  selector: 'eg-socket-type-tooltip-tool',
  templateUrl: './socket-type-tooltip-tool.component.html',
  styleUrls: ['./socket-type-tooltip-tool.component.scss']
})
export class SocketTypeToolTipTool extends ToolComponent implements OnInit {

  private readonly logger: ILogger;

  constructor(private graph: GraphComponent, private store: Store<GraphsState>, loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('SocketConnectionToolComponent');
  }

  ngOnInit() {
  }

}
