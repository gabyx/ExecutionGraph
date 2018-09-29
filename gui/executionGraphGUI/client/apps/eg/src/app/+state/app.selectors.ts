import { createFeatureSelector, createSelector } from '@ngrx/store';
import { AppState } from './app.state';

// Lookup the 'App' feature state managed by NgRx
const getAppState = createFeatureSelector<AppState>('app');

const getLoaded = createSelector(getAppState, (state: AppState) => state.loaded);
const getError = createSelector(getAppState, (state: AppState) => state.error);

const getAllGraphs = createSelector(getAppState, getLoaded, (state: AppState, isLoaded) => {
  return isLoaded ? state.graphs : undefined;
});

const getSelectedGraph = createSelector(getAppState, (state: AppState) => {
  return state.selectedGraph;
});

export const appQuery = {
  getLoaded,
  getError,
  getAllGraphs,
  getSelectedGraph
};
