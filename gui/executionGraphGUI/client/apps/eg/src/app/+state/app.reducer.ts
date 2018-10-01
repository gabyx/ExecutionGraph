import { AppAction, AppActionTypes } from './app.actions';
import { Graph, GraphTypeDescription, Node, Connection } from '../model';
import { Id, isDefined } from '@eg/common';
import { AppState } from './app.state';

export const initialState: AppState = new AppState();

export function appReducer(immutableState: AppState = initialState, action: AppAction): AppState {
  switch (action.type) {
    case AppActionTypes.AppLoaded: {
      let state = Object.assign(new AppState(), immutableState);
      // UI Props
      state.uiProps = action.uiProps;

      // Graphs
      action.graphs.forEach((graph: Graph) => {
        state.addGraph(graph);
      });
      action.graphDescriptions.forEach((graphDesc: GraphTypeDescription) => {
        state.addGraphDescription(graphDesc);
      });
      state.loaded = true;
      return state;
    }
    case AppActionTypes.UpdateUIProperties: {
      let state = Object.assign(new AppState(), immutableState, <AppState>{ uiProps: action.props });
      return state;
    }
    case AppActionTypes.SelectGraph: {
      let state = Object.assign(new AppState(), immutableState);
      state.selectedGraphId = action.id;
      return state;
    }
    case AppActionTypes.NodeMoved: {
      let state = Object.assign(new AppState(), immutableState);
      const graph = state.selectedGraph;
      if (isDefined(graph)) {
        let n = graph.node(action.node.id);
        if (isDefined(n)) {
          n.uiProps.position.x = action.newPosition.x;
          n.uiProps.position.y = action.newPosition.y;
        }
      }
      return state;
    }
    case AppActionTypes.NodeAdded: {
      let state = Object.assign(new AppState(), immutableState);
      const graph = state.selectedGraph;
      if (isDefined(graph)) {
        graph.addNode(action.node);
      }
      return state;
    }
    case AppActionTypes.NodeRemoved: {
      let state = Object.assign(new AppState(), immutableState);
      const graph = state.selectedGraph;
      if (isDefined(graph)) {
        graph.removeNode(action.id);
      }
      return state;
    }
    case AppActionTypes.ConnectionAdded: {
      let state = Object.assign(new AppState(), immutableState);
      const graph = state.selectedGraph;
      if (isDefined(graph)) {
        graph.addConnection(action.connection);
      }
      return state;
    }
    case AppActionTypes.ConnectionRemoved: {
      let state = Object.assign(new AppState(), immutableState);
      const graph = state.selectedGraph;
      if (isDefined(graph)) {
        graph.removeConnection(action.connectionId);
      }
      return state;
    }
    default:
      return immutableState;
  }
}
