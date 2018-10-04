import { createSelector } from '@ngrx/store';
import { getGraphsState } from './app.selectors';
import { GraphsState } from '../reducers/graph.reducers';

export const getGraphEntities = createSelector(
    getGraphsState,
    (state: GraphsState) => state.entities
);

export const getSelectedGraph = createSelector(
    getGraphsState,
    getGraphEntities,
    (state, entities) => state.selectedGraphId ? entities.get(state.selectedGraphId) : null
);

export const getGraphsLoaded = createSelector(
    getGraphsState,
    (state) => state.loaded
);