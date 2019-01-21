import { Action } from '@ngrx/store';
import { Id } from '@eg/common';
import {
  Graph,
  Node,
  Connection,
  NodeId,
  ConnectionId,
  GraphTypeDescription,
  NodeTypeDescription,
  Socket
} from '../../model';
import { Point, Position, MassSpringLayoutConfig } from '@eg/graph';

export const LOAD_GRAPHS = '[Graph] Load';
export const GRAPHS_LOADED = '[Graph] Loaded';
export const GRAPHS_LOAD_ERROR = '[Graph] Load Error';
export const OPEN_GRAPH = '[Graph] Open';
export const CREATE_GRAPH = '[Graph] Create';

export const MOVE_NODE = '[Graph] Move Node';
export const MOVE_NODES = '[Graph] Move Nodes';
export const NODES_MOVED = '[Graph] Nodes Moved';
export const NODE_UPDATED = '[Graph] Node Updated';
export const GRAPH_ADDED = '[Graph] Graph Added';
export const REMOVE_GRAPH = '[Graph] Remove Graph';
export const GRAPH_REMOVED = '[Graph] Graph Removed';
export const ADD_NODE = '[Graph] Add Node';
export const NODE_ADDED = '[Graph] Node Added';
export const REMOVE_NODE = '[Graph] Remove Node';
export const NODE_REMOVED = '[Graph] Node Removed';
export const ADD_CONNECTION = '[Graph] Add Connection';
export const CONNECTION_ADDED = '[Graph] Connection Added';
export const REMOVE_CONNECTION = '[Graph] Remove Connection';
export const CONNECTION_REMOVED = '[Graph] Connection Removed';

export const RUN_AUTO_LAYOUT_SPRING_SYSTEM = '[Graph] Run Auto Layout';

export class LoadGraphs implements Action {
  readonly type = LOAD_GRAPHS;
}

export class GraphLoadError implements Action {
  readonly type = GRAPHS_LOAD_ERROR;
  constructor(public error: any) {}
}

export class GraphsLoaded implements Action {
  readonly type = GRAPHS_LOADED;
  constructor(public graphs: Graph[]) {}
}

export class OpenGraph implements Action {
  readonly type = OPEN_GRAPH;
  constructor(public id: Id) {}
}

// Actions related to the Workspace
// --------------------------------
export class MoveNode implements Action {
  readonly type = MOVE_NODE;
  constructor(public node: Node, public newPosition: Position) {}
}
export class NodeUpdated implements Action {
  readonly type = NODE_UPDATED;
  constructor(public node: Node) {}
}

export type Moves = { node: Node; pos: Position }[];
export class MoveNodes implements Action {
  readonly type = MOVE_NODES;
  constructor(public moves: Moves) {}
}
export class NodesMoved implements Action {
  readonly type = NODES_MOVED;
}

// Actions related to GraphManagementService
// -----------------------------------------
export class CreateGraph implements Action {
  readonly type = CREATE_GRAPH;
  constructor(public graphType: GraphTypeDescription) {}
}

export class GraphAdded implements Action {
  readonly type = GRAPH_ADDED;
  constructor(public graph: Graph) {}
}

export class RemoveGraph implements Action {
  readonly type = REMOVE_GRAPH;
  constructor(public id: Id) {}
}

export class GraphRemoved implements Action {
  readonly type = GRAPH_REMOVED;
  constructor(public id: Id) {}
}

// Actions related to GraphManipulationService
// -------------------------------------------
export class AddNode implements Action {
  readonly type = ADD_NODE;
  constructor(public graphId: Id, public nodeType: NodeTypeDescription, public position?: Point) {}
}

export class NodeAdded implements Action {
  readonly type = NODE_ADDED;
  constructor(public graphId: Id, public node: Node) {}
}

export class RemoveNode implements Action {
  readonly type = REMOVE_NODE;
  constructor(public graphId: Id, public nodeId: NodeId) {}
}

export class NodeRemoved implements Action {
  readonly type = NODE_REMOVED;
  constructor(public graphId: Id, public nodeId: NodeId) {}
}

export class AddConnection implements Action {
  readonly type = ADD_CONNECTION;
  constructor(public graphId: Id, public source: Socket, public target: Socket, public cycleDetection: boolean) {}
}

export class ConnectionAdded implements Action {
  readonly type = CONNECTION_ADDED;
  constructor(public graphId: Id, public connection: Connection) {}
}

export class RemoveConnection implements Action {
  readonly type = REMOVE_CONNECTION;
  constructor(public graphId: Id, public connection: Connection) {}
}

export class ConnectionRemoved implements Action {
  readonly type = CONNECTION_REMOVED;
  constructor(public graphId: Id, public connectionId: ConnectionId) {}
}

// Actions related to AutoLayoutService
// -------------------------------------------
export class RunAutoLayoutSpringSystem implements Action {
  readonly type = RUN_AUTO_LAYOUT_SPRING_SYSTEM;
  constructor(public graph: Graph, public config?: MassSpringLayoutConfig) {}
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
  | MoveNodes
  | NodesMoved
  | NodeUpdated
  | AddNode
  | NodeAdded
  | RemoveNode
  | NodeRemoved
  | AddConnection
  | ConnectionAdded
  | RemoveConnection
  | ConnectionRemoved
  | RunAutoLayoutSpringSystem;
