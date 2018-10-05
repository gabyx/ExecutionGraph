import { Injectable } from '@angular/core';
import { Router, ActivatedRouteSnapshot } from '@angular/router';
import { Store } from '@ngrx/store';
import { Effect, Actions } from '@ngrx/effects';
import { ROUTER_NAVIGATION, RouterNavigationAction } from '@ngrx/router-store';

import { of, from, Observable } from 'rxjs';
import { map, tap, switchMap, catchError, filter, withLatestFrom, exhaustMap } from 'rxjs/operators';

import { Id } from '@eg/common';
import { LoggerFactory, ILogger } from '@eg/logger';

import * as fromGraph from '../actions/graph.actions';
import * as fromNotifications from '../actions/notification.actions';

import { createConnection } from '../../model';
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
        loggerFactory: LoggerFactory,
    ) {
        this.log = loggerFactory.create('AppEffects');
    }


    @Effect()
    loadGraphs$ = this.actions$.ofType<fromGraph.LoadGraphs>(fromGraph.LOAD_GRAPHS)
        .pipe(
            switchMap((action, state) => this.createDummyGraph()),
            catchError(error => {
                this.log.error(`Failed to load graphs`, error);
                return of(new fromGraph.GraphLoadError(error))
            })
        );

    @Effect()
    openingGraph$ = this.handleNavigation('graph/:graphId', (r, state) => of(new fromGraph.OpenGraph(new Id(r.params.graphId))));


    @Effect({ dispatch: false})
    openGraph$ = this.actions$.ofType<fromGraph.GraphAdded>(fromGraph.GRAPH_ADDED).
            pipe(
                tap(action => this.router.navigate(['graph', action.graph.id.toString()]))
            );

    @Effect()
    createGraph$ = this.actions$.ofType<fromGraph.CreateGraph>(fromGraph.CREATE_GRAPH)
        .pipe(
            switchMap((action, state) => from(this.graphManagementService.addGraph(action.graphType.id))),
            switchMap(graph => [
                new fromGraph.GraphAdded(graph),
                new fromNotifications.ShowNotification(`Shiny new graph created for you \u{1F6EB}`)
            ])
        );

    @Effect()
    moveNode$ = this.actions$.ofType<fromGraph.MoveNode>(fromGraph.MOVE_NODE)
    .pipe(
        tap((action) => { action.node.uiProps.position = { x: action.newPosition.x, y: action.newPosition.y }; }),
        map((action, state) => new fromGraph.NodeUpdated(action.node))
    );

    @Effect()
    addConnection$ = this.actions$.ofType<fromGraph.AddConnection>(fromGraph.ADD_CONNECTION)
        .pipe(
            map((action, state) => new fromGraph.ConnectionAdded(createConnection(action.source, action.target)))
        );

    private async createDummyGraph(): Promise<fromGraph.GraphsLoaded> {
        // Get Graph Infos
        const graphDescs = await this.generalInfoService.getAllGraphTypeDescriptions();
        const graphDesc = graphDescs[0];
        const graphTypeId = graphDesc.id;
        const nodeType = graphDesc.nodeTypeDescritptions[0].type;

        // Add a graph
        const graph = await this.graphManagementService.addGraph(graphTypeId);

        // Add nodes
        for (let i = 0; i < 3; ++i) {
            const node = await this.graphManipulationService.addNode(graph.id, nodeType, `${nodeType}-${i}`);
            graph.addNode(node);
        }

        return new fromGraph.GraphsLoaded([graph]);
    }

    private handleNavigation(path: string, callback: (a: RouterStateUrl, state: GraphsState) => Observable<any>) {
        const segments = path.split('/').map(s => s.trim());

        return this.actions$.ofType<RouterNavigationAction<RouterStateUrl>>(ROUTER_NAVIGATION).
            pipe(map(r => r.payload.routerState),
                 filter(r => r && arraysEqual(r.primaryRouteSegments, segments)),
                 withLatestFrom(this.store),
                 switchMap(([r, state]) => callback(r, state)),
                 catchError(error => {
                    this.log.error(`Failed to navigate`, error);
                    return of();
                 })
            );
    }

}
