import { createFeatureSelector, createSelector } from '@ngrx/store';
import * as fromRouter from '@ngrx/router-store';
import { RouterStateUrl, AppState } from '../reducers/app.reducers';
import { GraphsState } from '../reducers';
import { GraphDescriptionsState } from '../reducers/graphDescription.reducers';
import { UiState } from '../reducers/ui.reducers';

// Lookup the 'App' feature state managed by NgRx
//export const getAppState = createFeatureSelector<AppState>('app');

export const getRouterState = createFeatureSelector<fromRouter.RouterReducerState<RouterStateUrl>>('routerReducer');
export const getGraphsState = createFeatureSelector<GraphsState>('graphs');
export const getGraphDescriptionsState = createFeatureSelector<GraphDescriptionsState>('graphDescriptions');
export const getUiState = createFeatureSelector<UiState>('ui');

export const getDrawerRequired = createSelector(
  getRouterState,
  router => router && router.state && router.state.drawerRouteSegments.length > 0
);

// export const getLoaded = createSelector(getAppState, (state: AppState) => state.loaded);
// export const getError = createSelector(getAppState, (state: AppState) => state.error);
