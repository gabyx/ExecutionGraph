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

import { Component, OnInit, Injectable, ElementRef } from '@angular/core';
import { Observable } from 'rxjs';
import { filter, tap, map } from 'rxjs/operators';
import { Store } from '@ngrx/store';

import {
  Graph,
  Node,
  Connection,
  SocketIndex,
  InputSocket,
  OutputSocket,
  createConnection,
  isOutputSocket,
  NodeTypeDescription,
  Socket
} from '../../model';

import { ILogger, LoggerFactory } from '@eg/logger';
import { Point, ConnectionDrawStyle, EventSourceGateway, GraphComponent } from '@eg/graph';
import { isDefined } from '@eg/common';
import { GraphsState } from '../../+state/reducers';
import * as graphQueries from '../../+state/selectors/graph.selectors'
import { getConnectionDrawStyle, getSelection } from '../../+state/selectors/ui.selectors';
import { Selection, UiState } from '../../+state/reducers/ui.reducers';
import { CreateNode } from '../../+state/actions';

@Injectable()
@Component({
  selector: 'eg-workspace',
  templateUrl: './workspace.component.html',
  styleUrls: ['./workspace.component.scss']
})
export class WorkspaceComponent implements OnInit {
  private readonly logger: ILogger;

  private readonly selection: Observable<Selection>;

  public graph: Observable<Graph>;

  public readonly graphEvents = new EventSourceGateway<Graph>();

  public readonly nodeEvents = new EventSourceGateway<Node>();

  public readonly socketEvents = new EventSourceGateway<Socket>();

  public connectionDrawStyle: Observable<ConnectionDrawStyle>;

  public get nodes(): Observable<Node[]> {
    return this.graph.pipe(map(graph => graph.nodes), map(nodes => Object.keys(nodes).map(id => nodes[id])));
  }

  public get connections(): Observable<Connection[]> {
    return this.graph.pipe(
      map(graph => graph.connections),
      map(connections => Object.keys(connections).map(id => connections[id]))
    );
  }

  constructor(private store: Store<GraphsState>, uiStore: Store<UiState>, loggerFactory: LoggerFactory) {
    this.logger = loggerFactory.create('Workspace');
    this.selection = uiStore.select(getSelection);
    // this.graph.subscribe(g => this.logger.debug(`Displaying graph ${g.id}`));
  }

  ngOnInit() {
    this.graph = this.store.select(graphQueries.getSelectedGraph).pipe( filter(g => isDefined(g) && g!=null));
    this.connectionDrawStyle = this.store.select(getConnectionDrawStyle);
  }

  public createNode(nodeType: NodeTypeDescription, graph: Graph, position?: Point) {
    this.store.dispatch(new CreateNode(nodeType, graph.id, position))
  }

  public isNodeSelected(node: Node): Observable<boolean> {
    return this.selection.pipe(map(selection => selection.nodes.indexOf(node.id) >= 0));
  }

  public isNodeType(nodeType: NodeTypeDescription): boolean {
    return isDefined(nodeType.type) && isDefined(nodeType.type);
  }
}
