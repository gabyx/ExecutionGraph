import { Params, RouterStateSnapshot, Route, ActivatedRouteSnapshot } from '@angular/router';
import { ActionReducerMap } from '@ngrx/store';
import * as fromRouter from '@ngrx/router-store';

import { GraphsState, reducer as graphsReducer } from './graph.reducers';
import { GraphDescriptionsState, reducer as graphDescriptionsReducer } from './graphDescription.reducers';
import { RouterStateSerializer } from '@ngrx/router-store';
import { UiState, reducer as uiReducer } from './ui.reducers';

export interface RouterStateUrl {
  url: string;
  queryParams: Params;
  params: Params;
  primaryRouteSegments: string[];
  drawerRouteSegments: string[];
}

export class RouterStateUrlSerializer implements RouterStateSerializer<RouterStateUrl> {
  serialize(routerState: RouterStateSnapshot): RouterStateUrl {
    const { url } = routerState;
    const { queryParams } = routerState.root;

    const state = routerState.root;
    const primaryRouteSegments = [];
    const drawerRouteSegments = [];
    const primaryLeaf = this.traverseOutletRoutesToLeaf(state, 'primary', routeState => {
      if(routeState.routeConfig) {
        primaryRouteSegments.push(routeState.routeConfig.path);
      }
    });
    this.traverseOutletRoutesToLeaf(state, 'drawer', routeState => {
      if (routeState.routeConfig) {
        drawerRouteSegments.push(routeState.routeConfig.path);
      }
    });

    const { params } = primaryLeaf;

    return { url, queryParams, params, primaryRouteSegments, drawerRouteSegments};
  }

  private traverseOutletRoutesToLeaf(routeState: ActivatedRouteSnapshot, outletName: string = 'primary', callback: (route: ActivatedRouteSnapshot) => any): ActivatedRouteSnapshot {
    if (routeState.outlet === outletName) {
      callback(routeState);
    }
    if(routeState.children.length > 0) {
      for (const childState of routeState.children) {
        const leafRoute = this.traverseOutletRoutesToLeaf(childState, outletName, callback);
        if(leafRoute !== null) {
          return leafRoute;
        }
      }
      return null;
    } else if(routeState.outlet === outletName) {
      return routeState;
    }
    else {
      return null;
    }
  }

}

export interface AppState {
  routerReducer: fromRouter.RouterReducerState<RouterStateUrl>,
  graphs: GraphsState,
  graphDescriptions: GraphDescriptionsState,
  ui: UiState
}

export const reducers: ActionReducerMap<AppState> = {
  routerReducer: fromRouter.routerReducer,
  graphs: graphsReducer,
  graphDescriptions: graphDescriptionsReducer,
  ui: uiReducer
}
