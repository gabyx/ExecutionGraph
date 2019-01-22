import { isDefined } from '@eg/common';

import * as fromActions from '../actions/graph.actions';
import { Graph, Connection } from '../../model';
import { ConnectionMap, GraphId } from '../../model/Graph';

export interface GraphMap {
  [id: string]: Graph;
}

export interface GraphsState {
  entities: GraphMap;

  loaded: boolean; // Has the AppState been loaded
  error?: any; // Last none error (if any)
  selectedGraphId?: GraphId; // Which Graph has been selected
}

export const initialState: GraphsState = {
  entities: {},
  loaded: false
};

export function reducer(state: GraphsState = initialState, action: fromActions.GraphAction): GraphsState {
  switch (action.type) {
    case fromActions.GRAPHS_LOADED: {
      const entities = action.graphs.reduce(
        (existing: GraphMap, graph: Graph) => ({ ...existing, [graph.id]: graph }),
        { ...state.entities }
      );

      return {
        ...state,
        loaded: true,
        error: null,
        entities
      };
    }

    case fromActions.GRAPHS_LOAD_ERROR: {
      return {
        ...state,
        loaded: true,
        error: action.error
      };
    }

    case fromActions.GRAPH_ADDED: {
      const entities = { ...state.entities, [action.graph.id]: action.graph };

      return {
        ...state,
        entities
      };
    }

    case fromActions.OPEN_GRAPH: {
      return {
        ...state,
        selectedGraphId: action.id
      };
    }

    case fromActions.CONNECTION_ADDED:
    case fromActions.CONNECTION_REMOVED:
    case fromActions.NODE_ADDED:
    case fromActions.NODE_REMOVED: {
      if (!isDefined(state.selectedGraphId)) {
        throw new Error(`No active graph to operate on, ${action.type} cannot be handled`);
      }
      const graph = state.entities[state.selectedGraphId];
      if (!isDefined(graph)) {
        throw new Error(`No graph with the id ${state.selectedGraphId} exists`);
      }

      // Let the graph reducer handle the rest
      const updatedGraph = graphReducer(graph, action);
      return {
        ...state,
        entities: {
          ...state.entities,
          [state.selectedGraphId]: updatedGraph
        }
      };
    }
    default: {
      return state;
    }
  }
}

export function graphReducer(graph: Graph, action: fromActions.GraphAction): Graph {
  switch (action.type) {
    case fromActions.NODES_MOVED: {
      return graph;
    }
    case fromActions.NODE_UPDATED: {
      const { node } = action;
      console.log('node updated', node);
      return {
        ...graph,
        nodes: {
          ...graph.nodes,
          [node.id]: node
        }
      };
    }

    case fromActions.NODE_ADDED: {
      const { node } = action;
      return {
        ...graph,
        nodes: {
          ...graph.nodes,
          [node.id]: node
        }
      };
    }

    case fromActions.NODE_REMOVED: {
      // Remove by destructuring to the removed and the rest
      const { [action.nodeId]: removed, ...nodes } = graph.nodes;
      //@todo cmonspqr -> gabnue:
      // Either the backend should report removed connections due to
      // removed nodes to keep the model consistent, or the backend should
      // not hold the model at all but be stateless and merely execute graphs
      // that are passed in as a whole?
      // Anyway we deleting obsolete connections here is just a hack
      const connections = Object.keys(graph.connections)
        .map(id => graph.connections[id])
        .filter(connection => connection.inputSocket.parentId === action.nodeId)
        .filter(connection => connection.outputSocket.parentId === action.nodeId);
      return {
        ...graph,
        nodes: nodes,
        connections: connections.reduce(
          (existing: ConnectionMap, connection: Connection) => ({ ...existing, [connection.id]: connection }),
          {}
        )
      };
    }

    case fromActions.CONNECTION_ADDED: {
      const { connection } = action;
      return {
        ...graph,
        connections: {
          ...graph.connections,
          [connection.id]: connection
        }
      };
    }

    case fromActions.CONNECTION_REMOVED: {
      // Remove by destructuring to the removed and the rest
      const { [action.connectionId]: removed, ...connections } = graph.connections;
      return {
        ...graph,
        connections: connections
      };
    }
  }
}
