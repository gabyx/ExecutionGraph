import { AppAction, AppActionTypes } from './app.actions';
import { Graph } from '../model';
import { Id } from '@eg/common/src';

export interface AppState {
  graphs: Graph[]; // list of App; analogous to a sql normalized table
  selectedGraphId?: Id; // which App record has been selected
  loaded: boolean; // has the App list been loaded
  error?: any; // last none error (if any)
}

export const initialState: AppState = {
  graphs: [],
  loaded: false
};

export function appReducer(state: AppState = initialState, action: AppAction): AppState {
  switch (action.type) {
    case AppActionTypes.AppLoaded: {
      state = {
        ...state,
        graphs: action.payload,
        loaded: true
      };
      break;
    }
    case AppActionTypes.SelectGraph: {
      state = {
        ...state,
        selectedGraphId: action.id
      };
      break;
    }
    case AppActionTypes.ConnectionAdded: {
      const graph = state.graphs.find(g => g.id.equals(state.selectedGraphId));
      if(graph) {
        graph.connections.push(action.connection);
      }
      break;
    }
  }
  return state;
}
