import { Action } from '@ngrx/store';
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
import { GraphId } from '../../model/Graph';

export const LOAD_GRAPHS = '[Graphs] Load';
export const GRAPHS_LOADED = '[Graphs] Loaded';
export const GRAPHS_LOAD_ERROR = '[Graphs] Load Error';
export const OPEN_GRAPH = '[Graph] Open';

export const SAVE_GRAPH = '[Graph] Save';
export const GRAPH_SAVED = '[Graph] Saved';
export const LOAD_GRAPH = '[Graph] Load';
export const GRAPH_LOADED = '[Graph] Loaded';

export const MOVE_NODE = '[Graph] Move Node';
export const MOVE_NODES = '[Graph] Move Nodes';
export const NODES_MOVED = '[Graph] Nodes Moved';
export const NODE_UPDATED = '[Graph] Node Updated';

export const ADD_GRAPH = '[Graph] Add';
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
  constructor(public id: GraphId) {}
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
export class SaveGraph implements Action {
  readonly type = SAVE_GRAPH;
  constructor(public id: GraphId, public filePath: string, public overwrite: boolean) {}
}

export class GraphSaved implements Action {
  readonly type = GRAPH_SAVED;
  constructor(public id: GraphId) {}
}

export class LoadGraph implements Action {
  readonly type = LOAD_GRAPH;
  constructor(public filePath: string, public openAfterLoad: boolean) {}
}

export class GraphLoaded implements Action {
  readonly type = GRAPH_LOADED;
  constructor(public graph: Graph) {}
}

export class AddGraph implements Action {
  readonly type = ADD_GRAPH;
  constructor(public graphType: GraphTypeDescription) {}
}

export class GraphAdded implements Action {
  readonly type = GRAPH_ADDED;
  constructor(public graph: Graph) {}
}

export class RemoveGraph implements Action {
  readonly type = REMOVE_GRAPH;
  constructor(public id: GraphId) {}
}

export class GraphRemoved implements Action {
  readonly type = GRAPH_REMOVED;
  constructor(public id: GraphId) {}
}

// Actions related to GraphManipulationService
// -------------------------------------------
export class AddNode implements Action {
  readonly type = ADD_NODE;
  constructor(public graphId: GraphId, public nodeType: NodeTypeDescription, public position?: Point) {}
}

export class NodeAdded implements Action {
  readonly type = NODE_ADDED;
  constructor(public graphId: GraphId, public node: Node) {}
}

export class RemoveNode implements Action {
  readonly type = REMOVE_NODE;
  constructor(public graphId: GraphId, public nodeId: NodeId) {}
}

export class NodeRemoved implements Action {
  readonly type = NODE_REMOVED;
  constructor(public graphId: GraphId, public nodeId: NodeId) {}
}

export class AddConnection implements Action {
  readonly type = ADD_CONNECTION;
  constructor(public graphId: GraphId, public source: Socket, public target: Socket, public cycleDetection: boolean) {}
}

export class ConnectionAdded implements Action {
  readonly type = CONNECTION_ADDED;
  constructor(public graphId: GraphId, public connection: Connection) {}
}

export class RemoveConnection implements Action {
  readonly type = REMOVE_CONNECTION;
  constructor(public graphId: GraphId, public connection: Connection) {}
}

export class ConnectionRemoved implements Action {
  readonly type = CONNECTION_REMOVED;
  constructor(public graphId: GraphId, public connectionId: ConnectionId) {}
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
  | LoadGraph
  | GraphLoaded
  | SaveGraph
  | GraphSaved
  | AddGraph
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
