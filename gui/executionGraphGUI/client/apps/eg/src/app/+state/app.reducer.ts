import { AppAction, AppActionTypes } from './app.actions';
import { Graph, GraphTypeDescription, Node, Connection } from '../model';
import { Id, isDefined } from '@eg/common';
import { AppState } from './app.state';

export const initialState: AppState = new AppState();

export function appReducer(immutableState: AppState = initialState, action: AppAction): AppState {
  let state = Object.assign(new AppState(), immutableState);
  switch (action.type) {
    case AppActionTypes.AppLoaded: {
      action.graphs.forEach((graph: Graph) => {
        state.addGraph(graph);
      });
      action.graphDescriptions.forEach((graphDesc: GraphTypeDescription) => {
        state.addGraphDescription(graphDesc);
      });
      state.loaded = true;
      break;
    }
    case AppActionTypes.SelectGraph: {
      state.selectedGraphId = action.id;
      break;
    }
    case AppActionTypes.NodeMoved: {
      const graph = state.selectedGraph;
      if (isDefined(graph)) {
        let n = graph.node(action.node.id);
        if (isDefined(n)) {
          n.uiProps.position.x = action.newPosition.x;
          n.uiProps.position.y = action.newPosition.y;
        }
      }
      break;
    }
    case AppActionTypes.NodeAdded: {
      const graph = state.selectedGraph;
      if (isDefined(graph)) {
        graph.addNode(action.node);
      }
      break;
    }
    case AppActionTypes.NodeRemoved: {
      const graph = state.selectedGraph;
      if (isDefined(graph)) {
        graph.removeNode(action.id);
      }
      break;
    }
    case AppActionTypes.ConnectionAdded: {
      const graph = state.selectedGraph;
      if (isDefined(graph)) {
        graph.addConnection(action.connection);
      }
      break;
    }
    case AppActionTypes.ConnectionRemoved: {
      const graph = state.selectedGraph;
      if (isDefined(graph)) {
        graph.removeConnection(action.connectionId);
      }
      break;
    }
  }
  return state;
}
