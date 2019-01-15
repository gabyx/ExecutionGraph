import { Component, OnInit, Input } from '@angular/core';
import { Store } from '@ngrx/store';
import { Point, ConnectionDrawStyle, GraphComponent } from '@eg/graph';
import { ILogger, LoggerFactory } from '@eg/logger';

import { ToolComponent } from '../tool-component';
import { Id } from '@eg/common';
import { OutputSocket, InputSocket, Connection, Socket, SocketIndex } from '../../../model';
import { GraphsState } from '../../../+state/reducers';
import { AddConnection } from '../../../+state/actions';
import { getSelectedGraph } from '../../../+state/selectors';

@Component({
  selector: 'eg-socket-connection-tool',
  templateUrl: './socket-connection-tool.component.html',
  styleUrls: ['./socket-connection-tool.component.scss']
})
export class SocketConnectionToolComponent extends ToolComponent implements OnInit {
  @Input()
  connectionDrawStyle: ConnectionDrawStyle;

  public tempTargetSocket: InputSocket | OutputSocket = null;
  public tempConnection: Connection = null;
  public tempConnectionEndpoint = Point.zero.copy();

  public invalidity = Connection.Invalidity.Valid;
  public get invalidityMessages() {
    return Connection.getValidationErrors(this.invalidity);
  }

  private sourceSocket: Socket;
  private targetSocket: Socket;

  private selectedGraphId: Id;
  private readonly logger: ILogger;

  constructor(private graph: GraphComponent, private store: Store<GraphsState>, loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('SocketConnectionToolComponent');
    store.select(getSelectedGraph).subscribe(g => (this.selectedGraphId = g.id));
  }

  ngOnInit() {
    this.socketEvents.onDragStart.subscribe(e => {
      const socket = e.element;

      this.logger.info(`Initiating new connection from ${socket.idString}`);
      this.sourceSocket = socket;
      if (Socket.isOutputSocket(this.sourceSocket)) {
        this.tempTargetSocket = new InputSocket(socket.typeIndex, socket.name, new SocketIndex(0));
      } else {
        this.tempTargetSocket = new OutputSocket(socket.typeIndex, socket.name, new SocketIndex(0));
      }
      this.activate();
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
      this.deactivate();
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
          this.logger.error(`Invalid connection: ${Connection.getValidationErrors(this.invalidity).join(', ')}`);
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
        this.invalidity = Connection.Invalidity.Valid;
      }
    });
  }

  private abortConnection() {
    this.invalidity = Connection.Invalidity.Valid;
    this.tempConnection = null;
    this.tempTargetSocket = null;
    this.sourceSocket = null;
    this.targetSocket = null;
  }

  private addConnection(source: Socket, target: Socket) {
    // Create the connection
    this.store.dispatch(new AddConnection(this.selectedGraphId, source, target, false));
    this.abortConnection();
  }
}
