import { Injectable } from '@angular/core';
import { Effect, Actions } from '@ngrx/effects';

import { from, of } from 'rxjs';
import { map, tap, switchMap, catchError } from 'rxjs/operators';

import { LoggerFactory, ILogger } from '@eg/logger';

import { GeneralInfoService } from '../../services';
import * as fromGraphDescription from '../actions/graphDescription.actions';


@Injectable()
export class GraphDescriptionEffects {
    private readonly log: ILogger;

    constructor(
        private actions$: Actions,
        private readonly generalInfoService: GeneralInfoService,
        loggerFactory: LoggerFactory,
    ) {
        this.log = loggerFactory.create('AppEffects');
    }

    @Effect()
    loadGraphs$ = this.actions$.ofType<fromGraphDescription.LoadGraphDescriptions>(fromGraphDescription.LOAD_GRAPH_DESCRIPTIONS)
        .pipe(
            switchMap((action, state) => from(this.generalInfoService.getAllGraphTypeDescriptions())),
            tap((graphDescriptions) => this.log.info(`Loaded ${graphDescriptions.length} graph descriptions`)),
            map((graphDescriptions) => new fromGraphDescription.GraphDescriptionsLoaded(graphDescriptions)),
            catchError(error => {
                this.log.error(`Failed to load graph descriptions`, error);
                return of(new fromGraphDescription.GraphDescriptionsLoadError(error))
            })
        );
}
