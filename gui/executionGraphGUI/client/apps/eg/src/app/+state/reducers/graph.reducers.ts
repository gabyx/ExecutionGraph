import { Id, isDefined } from "@eg/common";

import * as fromActions from '../actions/graph.actions';
import { Graph } from "../../model";
import { NodeMap } from "../../model/Graph";

export interface GraphMap {
    [id: string]: Graph
}

export interface GraphsState {
    entities: GraphMap;

    loaded: boolean; // Has the AppState been loaded
    error?: any; // Last none error (if any)
    // @todo cmonspqr: Move this to the router
    selectedGraphId?: Id; // Which Graph has been selected
}

export const initialState: GraphsState = {
    entities: {},
    loaded: false
};

export function reducer(state: GraphsState = initialState, action: fromActions.GraphAction): GraphsState {
    switch (action.type) {

        case fromActions.GRAPHS_LOADED: {
            const entities = action.graphs.reduce(
                (existing: GraphMap, graph: Graph) => ({...existing, [graph.id.toString()]: graph}),
                {...state.entities});

            return {
                ...state,
                loaded: true,
                error: null,
                entities
            }
        }

        case fromActions.GRAPHS_LOAD_ERROR: {
            return {
                ...state,
                loaded: true,
                error: action.error
            }
        }

        case fromActions.GRAPH_ADDED: {
            const entities = { ...state.entities, [action.graph.id.toString()]: action.graph };

            return {
                ...state,
                entities
            }
        }

        case fromActions.OPEN_GRAPH: {
            return {
                ...state,
                selectedGraphId: action.id
            }
        }

        case fromActions.CONNECTION_ADDED:
        case fromActions.CONNECTION_REMOVED:
        case fromActions.NODE_ADDED:
        case fromActions.NODE_REMOVED:
        {
          if(!isDefined(state.selectedGraphId))
          {
              throw new Error(`No active graph to operate on, ${action.type} cannot be handled`);
          }
          const graph = state.entities[state.selectedGraphId.toString()];
          if(!isDefined(graph))
          {
              throw new Error(`No graph with the id ${state.selectedGraphId} exists`);
          }

          // Let the graph reducer handle the rest
          const updatedGraph = graphReducer(graph, action);
          console.log(updatedGraph);
          return {
            ...state,
            entities: {
              ...state.entities,
              [state.selectedGraphId.toString()]: updatedGraph
            }
          };
        }
      default: {
        return state;
      }

    }
};

export function graphReducer(graph: Graph, action: fromActions.GraphAction): Graph {

  switch(action.type) {

    case fromActions.NODE_UPDATED: {
      const { node } = action;
      return {
        ...graph,
        nodes: {
          ...graph.nodes,
          [node.id.toString()]: node
        }
      }
    }

    case fromActions.NODE_ADDED: {
      const { node } = action;
      return {
        ...graph,
        nodes: {
          ...graph.nodes,
          [node.id.toString()]: node
        }
      }
    }

    case fromActions.NODE_REMOVED: {
      // Remove by destructuring to the removed and the rest
      const {[action.id.toString()]: removed, ...nodes} = graph.nodes;
      return {
        ...graph,
        nodes: nodes
      }
    }

    case fromActions.CONNECTION_ADDED: {
      const { connection } = action;
      return {
        ...graph,
        connections: {
          ...graph.connections,
          [connection.idString]: connection
        }
      }
    }

    case fromActions.CONNECTION_REMOVED: {
        // Remove by destructuring to the removed and the rest
      const {[action.connectionId.string]: removed, ...connections} = graph.connections;
      return {
        ...graph,
        connections: connections
      }
    }
  }

}
