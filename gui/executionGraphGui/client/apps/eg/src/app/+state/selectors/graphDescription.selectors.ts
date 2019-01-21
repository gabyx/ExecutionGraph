import { createSelector } from '@ngrx/store';
import { getGraphDescriptionsState } from './app.selectors';
import { GraphDescriptionsState } from '../reducers/graphDescription.reducers';

export const getGraphDescriptionEntities = createSelector(
  getGraphDescriptionsState,
  (state: GraphDescriptionsState) => state.entities
);

export const getGraphDescriptions = createSelector(
  getGraphDescriptionEntities,
  entities => Object.keys(entities).map(id => entities[id])
);

export const getGraphDescriptionsLoaded = createSelector(
  getGraphDescriptionsState,
  state => state.loaded
);
