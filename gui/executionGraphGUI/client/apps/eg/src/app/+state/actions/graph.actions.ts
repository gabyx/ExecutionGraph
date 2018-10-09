import { Action } from '@ngrx/store';
import { Id } from '@eg/common';
import {
    Graph,
    Node,
    Connection,
    OutputSocket,
    InputSocket,
    NodeId,
    ConnectionId,
    GraphTypeDescription,
    NodeTypeDescription
} from '../../model';
import { Point } from '@eg/graph';

export const LOAD_GRAPHS = '[Graph] Load';
export const GRAPHS_LOADED = '[Graph] Loaded';
export const GRAPHS_LOAD_ERROR = '[Graph] Load Error';
export const OPEN_GRAPH = '[Graph] Open';
export const CREATE_GRAPH = '[Graph] Create';

export const MOVE_NODE = '[Graph] Move Node';
export const NODE_UPDATED = '[Graph] Node Updated';
export const GRAPH_ADDED = '[Graph] Graph Added';
export const REMOVE_GRAPH = '[Graph] Remove Graph';
export const GRAPH_REMOVED = '[Graph] Graph Removed';
export const CREATE_NODE = '[Graph] Create Node';
export const NODE_ADDED = '[Graph] Node Added';
export const REMOVE_NODE = '[Graph] Remove Node';
export const NODE_REMOVED = '[Graph] Node Removed';
export const ADD_CONNECTION = '[Graph] Add Connection';
export const CONNECTION_ADDED = '[Graph] Connection Added';
export const REMOVE_CONNECTION = '[Graph] Remove Connection';
export const CONNECTION_REMOVED = '[Graph] Connection Removed';


export class LoadGraphs implements Action {
    readonly type = LOAD_GRAPHS;
}

export class GraphLoadError implements Action {
    readonly type = GRAPHS_LOAD_ERROR;
    constructor(public error: any) { }
}

export class GraphsLoaded implements Action {
    readonly type = GRAPHS_LOADED;
    constructor(public graphs: Graph[]) { }
}

export class OpenGraph implements Action {
    readonly type = OPEN_GRAPH;
    constructor(public id: Id) { }
}

// Actions related to the Workspace
// --------------------------------
export class MoveNode implements Action {
    readonly type = MOVE_NODE;
    constructor(public node: Node, public newPosition: Point) { }
}
export class NodeUpdated implements Action {
    readonly type = NODE_UPDATED;
    constructor(public node: Node) { }
}

// Actions related to GraphManagementService
// -----------------------------------------
export class CreateGraph implements Action {
    readonly type = CREATE_GRAPH;
    constructor(public graphType: GraphTypeDescription) { }
}

export class GraphAdded implements Action {
    readonly type = GRAPH_ADDED;
    constructor(public graph: Graph) { }
}

export class RemoveGraph implements Action {
    readonly type = REMOVE_GRAPH;
    constructor(public id: Id) { }
}

export class GraphRemoved implements Action {
    readonly type = GRAPH_REMOVED;
    constructor(public id: Id) { }
}

// Actions related to GraphManipulationService
// -------------------------------------------
export class CreateNode implements Action {
    readonly type = CREATE_NODE;
    constructor(public nodeType: NodeTypeDescription, public graphId: Id, public position?: Point) { }
}

export class NodeAdded implements Action {
    readonly type = NODE_ADDED;
    constructor(public node: Node) { }
}

export class RemoveNode implements Action {
    readonly type = REMOVE_NODE;
    constructor(public id: NodeId) { }
}

export class NodeRemoved implements Action {
    readonly type = NODE_REMOVED;
    constructor(public id: NodeId) { }
}

export class AddConnection implements Action {
    readonly type = ADD_CONNECTION;
    constructor(public source: OutputSocket | InputSocket, public target: OutputSocket | InputSocket) { }
}

export class ConnectionAdded implements Action {
    readonly type = CONNECTION_ADDED;
    constructor(public connection: Connection) { }
}

export class RemoveConnection implements Action {
    readonly type = REMOVE_CONNECTION;
    constructor(public connection: Connection) { }
}

export class ConnectionRemoved implements Action {
    readonly type = CONNECTION_REMOVED;
    constructor(public connectionId: ConnectionId) { }
}

export type GraphAction =
    | LoadGraphs
    | GraphsLoaded
    | GraphLoadError
    | OpenGraph
    | CreateGraph
    | GraphAdded
    | RemoveGraph
    | GraphRemoved
    | MoveNode
    | NodeUpdated
    | CreateNode
    | NodeAdded
    | RemoveNode
    | NodeRemoved
    | AddConnection
    | ConnectionAdded
    | RemoveConnection
    | ConnectionRemoved;