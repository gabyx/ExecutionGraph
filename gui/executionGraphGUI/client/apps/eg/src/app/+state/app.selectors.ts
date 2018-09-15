import { createFeatureSelector, createSelector } from '@ngrx/store';
import { AppState } from './AppState';

// Lookup the 'App' feature state managed by NgRx
const getAppState = createFeatureSelector<AppState>('app');

const getLoaded = createSelector(getAppState, (state: AppState) => state.loaded);
const getError = createSelector(getAppState, (state: AppState) => state.error);

const getAllGraphs = createSelector(getAppState, getLoaded, (state: AppState, isLoaded) => {
  return isLoaded ? state.graphs : [];
});
const getSelectedGraph = createSelector(getAppState, (appState) => {
  const result = appState.getSelectedGraph();
  return result ? Object.assign({}, result) : undefined;
});

export const appQuery = {
  getLoaded,
  getError,
  getAllGraphs,
  getSelectedGraph
};
