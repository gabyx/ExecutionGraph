import { Id } from '@eg/common';
import { GraphTypeDescription } from '../../model';
import * as fromActions from '../actions/graphDescription.actions';

export interface GraphDescriptionMap {
  [id: string]: GraphTypeDescription;
}

export interface GraphDescriptionsState {
  entities: GraphDescriptionMap;
  loaded: boolean;
  error?: any;
}

export const initialState = {
  entities: {},
  loaded: false,
  error: null
};

export function reducer(
  state: GraphDescriptionsState = initialState,
  action: fromActions.GraphDescriptionActions
): GraphDescriptionsState {
  switch (action.type) {
    case fromActions.GRAPH_DESCRIPTIONS_LOADED: {
      const entities = action.graphDescriptions.reduce(
        (existing: GraphDescriptionMap, graphDescription: GraphTypeDescription) => ({
          ...existing,
          [graphDescription.id.id()]: graphDescription
        }),
        { ...state.entities }
      );

      return {
        ...state,
        loaded: true,
        error: null,
        entities
      };
    }

    case fromActions.GRAPH_DESCRIPTIONS_LOAD_ERROR: {
      return {
        ...state,
        loaded: true,
        error: action.error
      };
    }
  }
  return state;
}
