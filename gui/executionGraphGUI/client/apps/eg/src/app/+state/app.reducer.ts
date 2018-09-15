import { AppAction, AppActionTypes } from './app.actions';
import { Graph, GraphTypeDescription, Node, Connection } from '../model';
import { Id } from '@eg/common/src';
import { AppState } from "./AppState";

export const initialState: AppState = new AppState();

export function appReducer(state: AppState = initialState, action: AppAction): AppState {
  switch (action.type) {
    case AppActionTypes.AppLoaded: {
      action.graphs.forEach((graph: Graph) => {
        state.graphs[graph.id.toString()] = graph;
      });
      action.graphDescriptions.forEach((graphDesc: GraphTypeDescription) => {
        state.graphDescriptions[graphDesc.id.toString()] = graphDesc;
      })
      state.loaded = true;
      break;
    }
    case AppActionTypes.SelectGraph: {
      state.selectedGraphId = action.id
      break;
    }
    case AppActionTypes.NodeAdded: {
      const graph = state.getSelectedGraph();
      if (graph) {
        graph.nodes.push(action.node);
      }
      break;
    }
    case AppActionTypes.NodeRemoved: {
      const graph = state.getSelectedGraph();
      if (graph) {
        graph.nodes.filter((node: Node) => node.id.equals(action.id));
      }
      break;
    }
    case AppActionTypes.ConnectionAdded: {
      const graph = state.getSelectedGraph();
      if (graph) {
        graph.connections.push(action.connection);
      }
      break;
    }
    case AppActionTypes.ConnectionRemoved: {
      const graph = state.getSelectedGraph();
      if (graph) {
        graph.connections.filter((connection: Connection) => connection.id.equal(action.connectionId));
      }
      break;
    }
  }
  return state;
}
