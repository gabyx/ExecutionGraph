import { Component, OnInit, Input } from '@angular/core';
import { Store } from '@ngrx/store';
import { Point, ConnectionDrawStyle } from '@eg/graph';
import { ILogger, LoggerFactory } from '@eg/logger';

import { ToolComponent } from './tool-component';
import { OutputSocket, InputSocket, Connection, isOutputSocket, Socket, createConnection, SocketIndex } from '../../model';
import { GraphsState } from '../../+state/reducers';
import { AddConnection } from '../../+state/actions';

@Component({
    selector: 'eg-socket-connection-tool',
    template: `
    <ng-container *ngIf="newConnection">

        <ngcs-connection
          [from]="newConnection.outputSocket.idString"
          [to]="newConnection.inputSocket.idString"
          [drawStyle]="connectionDrawStyle">
        </ngcs-connection>

      <div style="position: absolute"
        [style.left]="newConnectionEndpoint.x+'px'"
        [style.top]="newConnectionEndpoint.y+'px'">

      <ngcs-port [id]="newTargetSocket.idString" class="right middle"></ngcs-port>

    </div>

    </ng-container>
    `,
    styles: [`
    `]
})
export class SocketConnectionToolComponent extends ToolComponent implements OnInit {

  @Input() connectionDrawStyle: ConnectionDrawStyle;

  public newTargetSocket: InputSocket | OutputSocket = null;
  public newConnection: Connection = null;
  public newConnectionEndpoint: Point = { x: 0, y: 0 };

  private readonly logger: ILogger;

  constructor(private store: Store<GraphsState>, loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('SocketConnectionToolComponent');
  }

  ngOnInit() {
    // (dragStarted)="initConnectionFrom(socket, graphComponent.convertMouseToGraphPosition($event.mousePosition))"
    // (dragContinued)="movingConnection(graphComponent.convertMouseToGraphPosition($event.mousePosition))"
    // (dragEnded)="abortConnection()"
    // [ngcsDroppable]="isInputSocket"
    // (draggableDropped)="addConnection($event.data, socket)">
    this.socketEvents.onDragStart.subscribe(e => this.initConnectionFrom(e.element, this.graph.convertMouseToGraphPosition(e.mousePosition)));
    this.socketEvents.onDragContinue.subscribe(e => this.movingConnection(this.graph.convertMouseToGraphPosition(e.mousePosition)));
    this.socketEvents.onDragStop.subscribe(e => this.abortConnection());
  }


  private initConnectionFrom(socket: Socket, position: Point) {
    this.logger.info(`Initiating new connection from ${socket.idString}`);
    if (isOutputSocket(socket)) {
      this.newTargetSocket = new InputSocket(socket.type, socket.name, new SocketIndex(0));
    } else {
      this.newTargetSocket = new OutputSocket(socket.type, socket.name, new SocketIndex(0));
    }
    // Create the connection
    this.newConnection = createConnection(socket, this.newTargetSocket);
    this.newConnectionEndpoint = position;
    console.log(this.newConnection);
  }

  private movingConnection(graphPosition: Point) {

    this.logger.info(`Setting position to ${this.newConnectionEndpoint.x}:${this.newConnectionEndpoint.y}`);
    this.newConnectionEndpoint = graphPosition;
  }

  private abortConnection() {
    this.newConnection = null;
  }

  private addConnection(source: OutputSocket | InputSocket, target: OutputSocket | InputSocket) {
    // Create the connection
    this.store.dispatch(new AddConnection(source, target));
  }


  private isOutputSocket(socket: InputSocket | OutputSocket): socket is OutputSocket {
    return isOutputSocket(socket);
  }

  private isInputSocket(socket: InputSocket | OutputSocket): socket is InputSocket {
    return !isOutputSocket(socket);
  }

}
