import { Injectable } from '@angular/core';
import { Router, ActivatedRouteSnapshot } from '@angular/router';
import { Store } from '@ngrx/store';
import { Effect, Actions, ofType } from '@ngrx/effects';
import { ROUTER_NAVIGATION, RouterNavigationAction } from '@ngrx/router-store';

import { of, from, Observable, merge as mergeObservables, throwError } from 'rxjs';
import { map, tap, catchError, filter, withLatestFrom, mergeMap } from 'rxjs/operators';

import { LoggerFactory, ILogger } from '@eg/logger';

import * as fromGraph from '../actions/graph.actions';
import * as fromNotifications from '../actions/notification.actions';

import { GraphManipulationService, GraphManagementService } from '../../services';
import { GraphsState } from '../reducers';
import { RouterStateUrl } from '../reducers/app.reducers';
import { arraysEqual, isDefined } from '@eg/common';
import { AutoLayoutService } from '../../services/AutoLayoutService';
import { Point } from '@eg/graph';
import { TestService } from '../../services/TestService';
import { Graph } from '../../model';
import { GraphSerializationService } from '../../services/GraphSerializationService';

@Injectable()
export class GraphEffects {
  private readonly log: ILogger;

  constructor(
    private actions$: Actions,
    private readonly router: Router,
    private readonly store: Store<GraphsState>,
    private readonly graphManipulationService: GraphManipulationService,
    private readonly graphManagementService: GraphManagementService,
    private readonly graphSerializationService: GraphSerializationService,
    private readonly autoLayoutService: AutoLayoutService,
    //! @todo gabnue->gabnue,simspoe This needs to be removed once the
    //! testing is finished.
    private readonly testService: TestService,
    loggerFactory: LoggerFactory
  ) {
    this.log = loggerFactory.create('AppEffects');
  }

  @Effect()
  loadGraphs$ = this.actions$.pipe(
    ofType<fromGraph.LoadGraphs>(fromGraph.LOAD_GRAPHS),
    mergeMap((action, state) =>
      from(this.testService.createDummyGraph()).pipe(map(g => new fromGraph.GraphsLoaded([g])))
    ),
    catchError(error => {
      this.log.error(`Failed to load graphs`, error);
      return of(new fromGraph.GraphLoadError(error));
    })
  );

  @Effect()
  openingGraph$ = this.handleNavigation('graph/:graphId', (r, state) => of(new fromGraph.OpenGraph(r.params.graphId)));

  @Effect({ dispatch: false })
  graphAdded$ = this.actions$.pipe(
    ofType<fromGraph.GraphAdded>(fromGraph.GRAPH_ADDED),
    tap(action => this.router.navigate(['graph', action.graph.id]))
  );

  @Effect({ dispatch: false })
  graphLoaded$ = this.actions$.pipe(
    ofType<fromGraph.GraphLoaded>(fromGraph.GRAPH_LOADED),
    tap(action => this.router.navigate(['graph', action.graph.id]))
  );

  @Effect()
  loadGraph$ = this.actions$.pipe(
    ofType<fromGraph.LoadGraph>(fromGraph.LOAD_GRAPH),
    mergeMap((action, state) =>
      from(this.graphSerializationService.loadGraph(action.filePath)).pipe(
        map(graph => <{ graph: Graph; filePath: string }>{ graph: graph, filePath: action.filePath })
      )
    ),
    mergeMap(l => [
      new fromGraph.GraphLoaded(l.graph),
      new fromNotifications.ShowNotification(`Graph id: '${l.graph.id}' loaded from '${l.filePath}' 🌻`, 4000)
    ])
  );

  @Effect()
  saveGraph$ = this.actions$.pipe(
    ofType<fromGraph.SaveGraph>(fromGraph.SAVE_GRAPH),
    mergeMap((action, state) =>
      from(this.graphSerializationService.saveGraph(action.id, action.filePath, action.overwrite)).pipe(
        map(() => [action.id, action.filePath])
      )
    ),
    mergeMap(([id, filePath]) => [
      new fromGraph.GraphSaved(id),
      new fromNotifications.ShowNotification(`Graph id: ${id} saved to '${filePath}' 🌻`, 4000)
    ])
  );

  @Effect()
  addGraph$ = this.actions$.pipe(
    ofType<fromGraph.AddGraph>(fromGraph.ADD_GRAPH),
    mergeMap((action, state) => from(this.graphManagementService.addGraph(action.graphType.id))),
    mergeMap(graph => [
      //@todo gabnue->gabnue Change here the name of the graph to some default value
      // dispatch new action fromGraph.GraphChangeProps(name: "...");
      new fromGraph.GraphAdded(graph),
      new fromNotifications.ShowNotification(`Shiny new graph created for you 👾`, 2000)
    ])
  );

  @Effect()
  addNode = this.actions$.pipe(
    ofType<fromGraph.AddNode>(fromGraph.ADD_NODE),
    mergeMap((action, state) =>
      from(this.graphManipulationService.addNode(action.graphId, action.nodeType.type, 'Node')).pipe(
        map(node => ({ node, action }))
      )
    ),
    mergeMap(({ node, action }) => [
      new fromGraph.NodeAdded(action.graphId, node),
      new fromGraph.MoveNode(node, action.position ? action.position : Point.zero.copy()),
      new fromNotifications.ShowNotification(`Added the node '${node.uiProps.name}' for you 👾`)
    ])
  );

  @Effect()
  removeNode$ = this.actions$.pipe(
    ofType<fromGraph.RemoveNode>(fromGraph.REMOVE_NODE),
    mergeMap(action =>
      from(this.graphManipulationService.removeNode(action.graphId, action.nodeId)).pipe(map(() => action))
    ),
    mergeMap(action => [
      new fromGraph.NodeRemoved(action.graphId, action.nodeId),
      new fromNotifications.ShowNotification(`Removed the node for you 👾`)
    ])
  );

  @Effect()
  moveNode$ = this.actions$.pipe(
    ofType<fromGraph.MoveNode>(fromGraph.MOVE_NODE),
    // tap(action => console.log('moving node ', action.node, action.newPosition)),
    tap(action => {
      action.node.uiProps.position = action.newPosition.copy();
    }),
    map((action, state) => new fromGraph.NodesMoved())
  );

  @Effect()
  moveNodes$ = this.actions$.pipe(
    ofType<fromGraph.MoveNodes>(fromGraph.MOVE_NODES),
    // tap(action => console.log('moving node ', action.node, action.newPosition)),
    tap(action => {
      action.moves.forEach(m => (m.node.uiProps.position = m.pos.copy()));
    }),
    // no real action to dispatch
    map((action, state) => new fromGraph.NodesMoved())
  );

  @Effect()
  addConnection$ = this.actions$.pipe(
    ofType<fromGraph.AddConnection>(fromGraph.ADD_CONNECTION),
    mergeMap(action =>
      from(
        this.graphManipulationService.addConnection(action.graphId, action.source, action.target, action.cycleDetection)
      ).pipe(map(conn => ({ conn, action })))
    ),
    map(({ conn, action }) => {
      return new fromGraph.ConnectionAdded(action.graphId, conn);
    }),
    catchError((error, caught) => {
      this.store.dispatch(new fromNotifications.ShowNotification(`Adding connection failed!: ${error}`, 5000));
      return caught;
    })
  );

  @Effect()
  layoutGraph$ = this.actions$.pipe(
    ofType<fromGraph.RunAutoLayoutSpringSystem>(fromGraph.RUN_AUTO_LAYOUT_SPRING_SYSTEM),
    mergeMap(action => from(this.autoLayoutService.layoutGraphSpringSystem(action.graph, action.config))),
    catchError((error, caught) => {
      this.store.dispatch(
        new fromNotifications.ShowNotification(
          'Ups, auto-layouting algorithm crashed. Sorry for the remaining noddle soup. 💣'
        )
      );
      return caught;
    }),
    map(() => new fromNotifications.ShowNotification('Tidied up the noddle soup only for you. 🍝'))
  );

  private handleNavigation(path: string, callback: (a: RouterStateUrl, state: GraphsState) => Observable<any>) {
    const segments = path.split('/').map(s => s.trim());

    return this.actions$.pipe(
      ofType<RouterNavigationAction<RouterStateUrl>>(ROUTER_NAVIGATION),
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
