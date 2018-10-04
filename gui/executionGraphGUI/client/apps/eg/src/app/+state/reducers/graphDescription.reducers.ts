import { Id } from "@eg/common";
import { GraphTypeDescription } from "../../model";
import * as fromActions from "../actions/graphDescription.actions";

export interface GraphDescriptionsState {
    entities: Map<Id, GraphTypeDescription>;
    loaded: boolean,
    error?: any
}

export const initialState = {
    entities: new Map<Id, GraphTypeDescription>(),
    loaded: false,
    error: null
}

export function reducer(state: GraphDescriptionsState = initialState, action: fromActions.GraphDescriptionActions): GraphDescriptionsState {
    switch (action.type) {

        case fromActions.GRAPH_DESCRIPTIONS_LOADED: {
            const entities = action.graphDescriptions.reduce(
                (existing: Map<Id, GraphTypeDescription>, graphDescription: GraphTypeDescription) => existing.set(graphDescription.id, graphDescription),
                { ...state.entities });

            return {
                ...state,
                loaded: true,
                error: null,
                entities
            }
            break;
        }

        case fromActions.GRAPH_DESCRIPTIONS_LOAD_ERROR: {
            return {
                ...state,
                loaded: true,
                error: action.error
            }
            break;
        }
    }
    return state;
}