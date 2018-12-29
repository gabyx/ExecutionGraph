import { Action } from "@ngrx/store";
import { GraphTypeDescription } from "../../model";

export const LOAD_GRAPH_DESCRIPTIONS = '[GraphDescriptions] Load';
export const GRAPH_DESCRIPTIONS_LOADED = '[GraphDescriptions] Loaded';
export const GRAPH_DESCRIPTIONS_LOAD_ERROR = '[GraphDescriptions] Load Error';

export class LoadGraphDescriptions implements Action {
    readonly type = LOAD_GRAPH_DESCRIPTIONS;
}

export class GraphDescriptionsLoadError implements Action {
    readonly type = GRAPH_DESCRIPTIONS_LOAD_ERROR;
    constructor(public error: any) { }
}

export class GraphDescriptionsLoaded implements Action {
    readonly type = GRAPH_DESCRIPTIONS_LOADED;
    constructor(public graphDescriptions: GraphTypeDescription[]) { }
}

export type GraphDescriptionActions =
    | LoadGraphDescriptions
    | GraphDescriptionsLoaded
    | GraphDescriptionsLoadError
;