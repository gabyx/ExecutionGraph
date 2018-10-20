import { Component, OnInit, Input, ChangeDetectorRef } from '@angular/core';
import { Store } from '@ngrx/store';
import { Point, ConnectionDrawStyle, GraphComponent } from '@eg/graph';
import { ILogger, LoggerFactory } from '@eg/logger';

import { ToolComponent } from './tool-component';
import {
  OutputSocket,
  InputSocket,
  Connection,
  isOutputSocket,
  Socket,
  createConnection,
  SocketIndex
} from '../../model';
import { GraphsState } from '../../+state/reducers';
import { AddConnection } from '../../+state/actions';

@Component({
  selector: 'eg-socket-connection-tool',
  template: `
    <ng-container *ngIf="tempConnection">

      <eg-connection-layer
          [connections]="[tempConnection]"
          [drawStyle]="connectionDrawStyle"
          class="new-connections"
          [class.nondroppable]="!targetSocket">
      </eg-connection-layer>

      <ngcs-html-layer>
        <div
        *ngIf="!targetSocket"
        style="position: absolute"
        [style.left]="tempConnectionEndpoint.x+'px'"
        [style.top]="tempConnectionEndpoint.y+'px'">

          <ngcs-port [id]="tempTargetSocket.idString" class="right middle"></ngcs-port>

        </div>
      </ngcs-html-layer>
    </ng-container>
    `,
  styles: [
    `
      ::ng-deep {
        .new-connections.nondroppable .ngcs-connection {
          stroke: #888;
        }
      }
    `
  ]
})
export class SocketConnectionToolComponent extends ToolComponent implements OnInit {
  @Input()
  connectionDrawStyle: ConnectionDrawStyle;

  public tempTargetSocket: InputSocket | OutputSocket = null;
  public tempConnection: Connection = null;
  public tempConnectionEndpoint: Point = { x: 0, y: 0 };

  private sourceSocket: Socket;
  private targetSocket: Socket;

  private readonly logger: ILogger;

  constructor(private graph: GraphComponent, private store: Store<GraphsState>, loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('SocketConnectionToolComponent');
  }

  ngOnInit() {
    this.socketEvents.onDragStart.subscribe(e => {
      const socket = e.element;

      this.logger.info(`Initiating new connection from ${socket.idString}`);
      this.sourceSocket = socket;
      if (isOutputSocket(this.sourceSocket)) {
        this.tempTargetSocket = new InputSocket(socket.type, socket.name, new SocketIndex(0));
      } else {
        this.tempTargetSocket = new OutputSocket(socket.type, socket.name, new SocketIndex(0));
      }

      this.tempConnectionEndpoint = this.graph.convertMouseToGraphPosition(e.mousePosition);
      this.tempConnection = createConnection(socket, this.tempTargetSocket);
    });
    this.socketEvents.onDragContinue.subscribe(e => {
      if (!this.targetSocket) {
        this.tempConnectionEndpoint = this.graph.convertMouseToGraphPosition(e.mousePosition);
      }
    });
    this.socketEvents.onDragStop.subscribe(e => {
      if (this.targetSocket) {
        this.addConnection(this.sourceSocket, this.targetSocket);
      } else {
        this.abortConnection();
      }
    });

    this.socketEvents.onEnter.subscribe(e => {
      if (this.tempConnection) {
        const targetSocket = e.element;
        if (targetSocket !== this.sourceSocket && this.sourceSocket.kind !== targetSocket.kind) {
          console.log('Entering potential target socket');
          this.targetSocket = targetSocket;
          this.tempConnection = createConnection(this.sourceSocket, this.targetSocket);
        }
      }
    });

    this.socketEvents.onLeave.subscribe(e => {
      if (this.targetSocket) {
        console.log('Leaving potential target Socket');
        this.targetSocket = null;
        this.tempConnection = createConnection(this.sourceSocket, this.tempTargetSocket);
        this.tempConnectionEndpoint = this.graph.convertMouseToGraphPosition(e.mousePosition);
      }
    });
  }

  private abortConnection() {
    this.tempConnection = null;
    this.tempTargetSocket = null;
    this.sourceSocket = null;
    this.targetSocket = null;
  }

  private addConnection(source: Socket, target: Socket) {
    // Create the connection
    this.store.dispatch(new AddConnection(source, target));
    this.abortConnection();
  }
}
