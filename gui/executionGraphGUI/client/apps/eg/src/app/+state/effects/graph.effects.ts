import { Injectable } from '@angular/core';
import { Effect, Actions } from '@ngrx/effects';

import { of, from } from 'rxjs';
import { map, tap, switchMap, catchError } from 'rxjs/operators';

import { LoggerFactory, ILogger } from '@eg/logger';

import * as fromGraph from '../actions/graph.actions';

import { createConnection } from '../../model';
import { GeneralInfoService, GraphManipulationService, GraphManagementService } from '../../services';
import { Router } from '@angular/router';

@Injectable()
export class GraphEffects {
    private readonly log: ILogger;

    constructor(
        private actions$: Actions,
        private readonly router: Router,
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

    @Effect({ dispatch: false })
    openGraph$ = this.actions$.ofType<fromGraph.OpenGraph>(fromGraph.OPEN_GRAPH)
        .pipe(
            tap(action => { this.router.navigate(['graph', action.id.toString()]); })
        );

    @Effect()
    createGraph$ = this.actions$.ofType<fromGraph.CreateGraph>(fromGraph.CREATE_GRAPH)
        .pipe(
            switchMap((action, state) => from(this.graphManagementService.addGraph(action.graphType.id))),
            switchMap(graph => [
                new fromGraph.GraphsLoaded([graph]),
                new fromGraph.OpenGraph(graph.id)
            ])
        );

    @Effect()
    moveNode$ = this.actions$.ofType<fromGraph.MoveNode>(fromGraph.MOVE_NODE)
    .pipe(
        tap((action) => { action.node.uiProps.position = { x: action.newPosition.x, y: action.newPosition.y }; }),
        map((action, state) => new fromGraph.NodeUpdated(action.node))
        //map((action) => ({ ...action.node, uiProps: { position: action.newPosition }})),
        // map((updatedNode) => new fromGraph.NodeUpdated(updatedNode))
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
}
