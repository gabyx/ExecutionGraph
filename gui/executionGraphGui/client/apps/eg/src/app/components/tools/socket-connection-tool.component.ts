import { Component, OnInit, Input, ChangeDetectorRef } from '@angular/core';
import { Store } from '@ngrx/store';
import { Point, ConnectionDrawStyle, GraphComponent } from '@eg/graph';
import { ILogger, LoggerFactory } from '@eg/logger';

import { ToolComponent } from './tool-component';
import { OutputSocket, InputSocket, Connection, Socket, SocketIndex } from '../../model';
import { GraphsState } from '../../+state/reducers';
import { AddConnection } from '../../+state/actions';

@Component({
  selector: 'eg-socket-connection-tool',
  template: `
    <ng-container *ngIf="tempConnection">

      <eg-connection-layer
          [connections]="[tempConnection]"
          [drawStyle]="connectionDrawStyle"
          class="temp"
          [class.invalid]="invalidity"
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
          <div *ngIf="invalidity" class="invalidity-tooltip"
            [style.left]="tempConnectionEndpoint.x+'px'"
            [style.top]="tempConnectionEndpoint.y+'px'">
            <ul>
              <li *ngFor="let message of invalidityMessages">{{message}}</li>
            </ul>
          </div>
      </ngcs-html-layer>
    </ng-container>
    `,
  styleUrls: ['./socket-connection-tool.component.scss']
})
export class SocketConnectionToolComponent extends ToolComponent implements OnInit {
  @Input()
  connectionDrawStyle: ConnectionDrawStyle;

  public tempTargetSocket: InputSocket | OutputSocket = null;
  public tempConnection: Connection = null;
  public tempConnectionEndpoint: Point = { x: 0, y: 0 };

  public invalidity = Connection.Validity.Valid;
  public get invalidityMessages() {
    return Connection.getValidationErrors(this.invalidity);
  }

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
      if (Socket.isOutputSocket(this.sourceSocket)) {
        this.tempTargetSocket = new InputSocket(socket.type, socket.name, new SocketIndex(0));
      } else {
        this.tempTargetSocket = new OutputSocket(socket.type, socket.name, new SocketIndex(0));
      }

      this.tempConnectionEndpoint = this.graph.convertMouseToGraphPosition(e.mousePosition);
      this.tempConnection = Connection.create(socket, this.tempTargetSocket);
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
      this.logger.info('onEnter');
      if (this.tempConnection) {
        const targetSocket = e.element;
        this.invalidity = Connection.isInvalid(this.sourceSocket, targetSocket);

        if (!this.invalidity) {
          this.logger.info('Making preview connection');
          this.targetSocket = targetSocket;
          this.tempConnection = Connection.create(this.sourceSocket, this.targetSocket, false);
        } else {
          /** Add notifcation icon next to cursor, to make
           *  clear that this connection is impossible
           */
          this.logger.error(`Invalid connection: ${Connection.getValidationErrors(this.invalidity).join(", ")}`);
        }
      }
    });

    this.socketEvents.onLeave.subscribe(e => {
      this.logger.info('onLeave');
      if (this.targetSocket) {
        this.logger.info('Leaving potential target Socket');
        this.targetSocket = null;
        this.tempConnection = Connection.create(this.sourceSocket, this.tempTargetSocket);
        this.tempConnectionEndpoint = this.graph.convertMouseToGraphPosition(e.mousePosition);
      }
      if (this.tempConnection) {
        this.invalidity = Connection.Validity.Valid;
      }
    });
  }

  private abortConnection() {
    this.invalidity = Connection.Validity.Valid;
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
