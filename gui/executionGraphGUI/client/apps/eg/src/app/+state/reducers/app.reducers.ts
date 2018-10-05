import { Params, RouterStateSnapshot, Route } from '@angular/router';
import { ActionReducerMap } from '@ngrx/store';
import * as fromRouter from '@ngrx/router-store';

import { GraphsState, reducer as graphsReducer } from './graph.reducers';
import { GraphDescriptionsState, reducer as graphDescriptionsReducer } from './graphDescription.reducers';
import { RouterStateSerializer } from '@ngrx/router-store';

export interface RouterStateUrl {
  url: string;
  queryParams: Params;
  params: Params;
  routeSegments: string[]
}

export class RouterStateUrlSerializer implements RouterStateSerializer<RouterStateUrl> {
  serialize(routerState: RouterStateSnapshot): RouterStateUrl {
    const { url } = routerState;
    const { queryParams } = routerState.root;

    let state = routerState.root;
    const routeSegments = [];
    while(state.firstChild) {
      state = state.firstChild;
      if(state.routeConfig) {
        routeSegments.push(state.routeConfig.path);
      }
    }

    const { params } = state;

    return { url, queryParams, params, routeSegments};
  }
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
