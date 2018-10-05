import { Id, isDefined } from "@eg/common";

import * as fromActions from '../actions/graph.actions';
import { Graph } from "../../model";

export interface GraphsState {
    entities: Map<Id, Graph>;

    loaded: boolean; // Has the AppState been loaded
    error?: any; // Last none error (if any)
    // @todo cmonspqr: Move this to the router
    selectedGraphId?: Id; // Which Graph has been selected
}

export const initialState: GraphsState = {
    entities: new Map<Id, Graph>(),
    loaded: false
};

export function reducer(state: GraphsState = initialState, action: fromActions.GraphAction): GraphsState {
    switch (action.type) {

        case fromActions.GRAPHS_LOADED: {
            const entities = action.graphs.reduce(
                (existing: Map<Id, Graph>, graph: Graph) => existing.set(graph.id, graph),
                new Map<Id, Graph>(state.entities));

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

        case fromActions.SELECT_GRAPH: {
            return {
                ...state,
                selectedGraphId: action.id
            };
        }

        case fromActions.NODE_UPDATED: {
            if(!isDefined(state.selectedGraphId))
            {
                throw new Error('No active graph to move a node on');
            }

            //@todo cmonspqr: This is not proper immutable state
            const graph = state.entities.get(state.selectedGraphId);
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
            state.entities.get(state.selectedGraphId).addNode(action.node);
            return {
                ...state
            };
        }

        case fromActions.NODE_REMOVED: {
            if (!isDefined(state.selectedGraphId)) {
                throw new Error('No active graph to remove a node from');
            }
            //@todo cmonspqr: This is not proper immutable state
            state.entities.get(state.selectedGraphId).removeNode(action.id);
            return {
                ...state
            };
        }

        case fromActions.CONNECTION_ADDED: {
            if (!isDefined(state.selectedGraphId)) {
                throw new Error('No active graph to add a connection to');
            }
            //@todo cmonspqr: This is not proper immutable state
            state.entities.get(state.selectedGraphId).addConnection(action.connection);
            return {
                ...state
            };
        }

        case fromActions.CONNECTION_REMOVED: {
            if (!isDefined(state.selectedGraphId)) {
                throw new Error('No active graph to remove a connection from');
            }
            //@todo cmonspqr: This is not proper immutable state
            state.entities.get(state.selectedGraphId).removeConnection(action.connectionId);
            return {
                ...state
            };
        }

        default:
            return state;
    }
};