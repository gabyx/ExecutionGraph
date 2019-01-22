import { createSelector } from '@ngrx/store';
import { getGraphsState, getRouterState } from './app.selectors';
import { GraphsState, GraphMap } from '../reducers/graph.reducers';
import { isDefined } from '@angular/compiler/src/util';

export const getGraphEntities = createSelector(
  getGraphsState,
  (state: GraphsState) => state.entities
);

export const getGraphs = createSelector(
  getGraphEntities,
  (entities: GraphMap) => Object.keys(entities).map(id => entities[id])
);

// export const getSelectedGraphId = createSelector(
//     getRouterState,
//     (router) => router.state && router.state.params.graphId
// )

export const getSelectedGraphId = createSelector(
  getGraphsState,
  state => state.selectedGraphId
);

export const getSelectedGraph = createSelector(
  getSelectedGraphId,
  getGraphEntities,
  (id, entities) => (isDefined(id) ? entities[id] : null)
);

export const getGraphsLoaded = createSelector(
  getGraphsState,
  state => state.loaded
);
