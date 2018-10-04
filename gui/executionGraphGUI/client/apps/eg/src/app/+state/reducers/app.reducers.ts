import { Params } from '@angular/router';
import { ActionReducerMap } from '@ngrx/store';
import * as fromRouter from '@ngrx/router-store';

import { GraphsState, reducer as graphsReducer } from './graph.reducers';
import { GraphDescriptionsState, reducer as graphDescriptionsReducer } from './graphDescription.reducers';

export interface RouterStateUrl {
  url: string;
  queryParams: Params;
  params: Params;
}

export interface AppState {
  routerReducer: fromRouter.RouterReducerState<RouterStateUrl>,
  graphs: GraphsState,
  graphDescriptions: GraphDescriptionsState
}

export const reducers: ActionReducerMap<AppState> = {
  routerReducer: fromRouter.routerReducer,
  graphs: graphsReducer,
  graphDescriptions: graphDescriptionsReducer
}
