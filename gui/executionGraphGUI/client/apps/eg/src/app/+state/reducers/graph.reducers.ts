import { Id, isDefined } from "@eg/common";

import * as fromActions from '../actions/graph.actions';
import { Graph } from "../../model";

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
            break;
        }

        case fromActions.GRAPHS_LOAD_ERROR: {
            return {
                ...state,
                loaded: true,
                error: action.error
            }
            break;
        }

        case fromActions.NODE_UPDATED: {
            if(!isDefined(state.selectedGraphId))
            {
                throw new Error('No active graph to move a node on');
            }

            //@todo cmonspqr: This is not proper immutable state
            const graph = state.entities[state.selectedGraphId.toString()];
            graph.removeNode(action.node.id);
            graph.addNode(action.node);
            return {
                ...state
            };
        }

        case fromActions.NODE_ADDED: {
            if (!isDefined(state.selectedGraphId)) {
                throw new Error('No active graph to add a node to');
            }
            //@todo cmonspqr: This is not proper immutable state
            const graph = state.entities[state.selectedGraphId.toString()];
            graph.addNode(action.node);
            return {
                ...state
            };
        }

        case fromActions.NODE_REMOVED: {
            if (!isDefined(state.selectedGraphId)) {
                throw new Error('No active graph to remove a node from');
            }
            //@todo cmonspqr: This is not proper immutable state
            const graph = state.entities[state.selectedGraphId.toString()];
            graph.removeNode(action.id);
            return {
                ...state
            };
        }

        case fromActions.CONNECTION_ADDED: {
            if (!isDefined(state.selectedGraphId)) {
                throw new Error('No active graph to add a connection to');
            }
            //@todo cmonspqr: This is not proper immutable state
            const graph = state.entities[state.selectedGraphId.toString()];
            graph.addConnection(action.connection);
            return {
                ...state
            };
        }

        case fromActions.CONNECTION_REMOVED: {
            if (!isDefined(state.selectedGraphId)) {
                throw new Error('No active graph to remove a connection from');
            }
            //@todo cmonspqr: This is not proper immutable state
            const graph = state.entities[state.selectedGraphId.toString()];
            graph.removeConnection(action.connectionId);
            return {
                ...state
            };
        }

        default:
            return state;
    }
};