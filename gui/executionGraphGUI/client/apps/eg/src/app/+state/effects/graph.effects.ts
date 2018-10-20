import { Injectable } from '@angular/core';
import { Router, ActivatedRouteSnapshot } from '@angular/router';
import { Store } from '@ngrx/store';
import { Effect, Actions } from '@ngrx/effects';
import { ROUTER_NAVIGATION, RouterNavigationAction } from '@ngrx/router-store';

import { of, from, Observable } from 'rxjs';
import { map, tap, switchMap, catchError, filter, withLatestFrom, exhaustMap, mergeMap } from 'rxjs/operators';

import { Id } from '@eg/common';
import { LoggerFactory, ILogger } from '@eg/logger';

import * as fromGraph from '../actions/graph.actions';
import * as fromNotifications from '../actions/notification.actions';

import { Connection, Node } from '../../model';
import { GeneralInfoService, GraphManipulationService, GraphManagementService } from '../../services';
import { GraphsState } from '../reducers';
import { RouterStateUrl } from '../reducers/app.reducers';
import { arraysEqual } from '@eg/common';

@Injectable()
export class GraphEffects {
  private readonly log: ILogger;

  constructor(
    private actions$: Actions,
    private readonly router: Router,
    private readonly store: Store<GraphsState>,
    private readonly generalInfoService: GeneralInfoService,
    private readonly graphManipulationService: GraphManipulationService,
    private readonly graphManagementService: GraphManagementService,
    loggerFactory: LoggerFactory
  ) {
    this.log = loggerFactory.create('AppEffects');
  }

  @Effect()
  loadGraphs$ = this.actions$.ofType<fromGraph.LoadGraphs>(fromGraph.LOAD_GRAPHS).pipe(
    mergeMap((action, state) => this.createDummyGraph()),
    catchError(error => {
      this.log.error(`Failed to load graphs`, error);
      return of(new fromGraph.GraphLoadError(error));
    })
  );

  @Effect()
  openingGraph$ = this.handleNavigation('graph/:graphId', (r, state) =>
    of(new fromGraph.OpenGraph(new Id(r.params.graphId)))
  );

  @Effect({ dispatch: false })
  openGraph$ = this.actions$
    .ofType<fromGraph.GraphAdded>(fromGraph.GRAPH_ADDED)
    .pipe(tap(action => this.router.navigate(['graph', action.graph.id.toString()])));

  @Effect()
  createGraph$ = this.actions$.ofType<fromGraph.CreateGraph>(fromGraph.CREATE_GRAPH).pipe(
    mergeMap((action, state) => from(this.graphManagementService.addGraph(action.graphType.id))),
    mergeMap(graph => [
      new fromGraph.GraphAdded(graph),
      new fromNotifications.ShowNotification(`Shiny new graph created for you \u{1F6EB}`)
    ])
  );

  @Effect()
  createNode$ = this.actions$.ofType<fromGraph.CreateNode>(fromGraph.CREATE_NODE).pipe(
    mergeMap((action, state) =>
      from(this.graphManipulationService.addNode(action.graphId, action.nodeType.name, 'Some new node')).pipe(
        map(node => ({ node, action }))
      )
    ),
    mergeMap(({ node, action }) => [
      new fromGraph.NodeAdded(node),
      new fromGraph.MoveNode(node, action.position ? action.position : { x: 0, y: 0 }),
      new fromNotifications.ShowNotification(`Added the node ${node.name} for you \u{1F6EB}`)
    ])
  );

  @Effect()
  moveNode$ = this.actions$.ofType<fromGraph.MoveNode>(fromGraph.MOVE_NODE).pipe(
    // tap(action => console.log('moving node ', action.node, action.newPosition)),
    tap(action => {
      action.node.uiProps.position = { x: action.newPosition.x, y: action.newPosition.y };
    }),
    map((action, state) => new fromGraph.NodeUpdated(action.node))
  );

  @Effect()
  removeNode$ = this.actions$.ofType<fromGraph.RemoveNode>(fromGraph.REMOVE_NODE).pipe(
    mergeMap(action =>
      from(this.graphManipulationService.removeNode(action.graphId, action.nodeId)).pipe(map(() => action))
    ),
    mergeMap(action => [
      new fromGraph.NodeRemoved(action.nodeId),
      new fromNotifications.ShowNotification(`Removed the node for you \u{1F6EB}`)
    ])
  );

  @Effect()
  addConnection$ = this.actions$
    .ofType<fromGraph.AddConnection>(fromGraph.ADD_CONNECTION)
    .pipe(
      map((action, state) => new fromGraph.ConnectionAdded(Connection.createConnection(action.source, action.target)))
    );

  private async createDummyGraph(): Promise<fromGraph.GraphsLoaded> {
    // Get Graph Infos
    const graphDescs = await this.generalInfoService.getAllGraphTypeDescriptions();
    const graphDesc = graphDescs[0];
    const graphTypeId = graphDesc.id;
    const nodeType = 'Normalize';

    // Add a graph
    let graph = await this.graphManagementService.addGraph(graphTypeId);
    const nodes = {};
    // Add nodes
    let lastNode: Node = null;
    const connections = {};
    for (let i = 0; i < 3; ++i) {
      const node = await this.graphManipulationService.addNode(graph.id, nodeType, `${i}`);
      node.uiProps.position.x = 200 * i;
      node.uiProps.position.y = 50 + 100 * i;
      nodes[node.id.toString()] = node;

      if (lastNode) {
        const connection = Connection.createConnection(lastNode.outputs[0], node.inputs[0]);
        connections[connection.idString] = connection;
      }
      lastNode = node;
    }

    graph = { ...graph, nodes: nodes, connections: connections };

    return new fromGraph.GraphsLoaded([graph]);
  }

  private handleNavigation(path: string, callback: (a: RouterStateUrl, state: GraphsState) => Observable<any>) {
    const segments = path.split('/').map(s => s.trim());

    return this.actions$.ofType<RouterNavigationAction<RouterStateUrl>>(ROUTER_NAVIGATION).pipe(
      map(r => r.payload.routerState),
      filter(r => r && arraysEqual(r.primaryRouteSegments, segments)),
      withLatestFrom(this.store),
      //tap(([r, state]) => console.log(`Handling navigation`)))
      mergeMap(([r, state]) => callback(r, state)),
      catchError(error => {
        this.log.error(`Failed to navigate`, error);
        return of();
      })
    );
  }
}
