import { Action } from '@ngrx/store';
import { Id } from '@eg/common/src';
import { Graph, Node, Connection, OutputSocket, InputSocket, NodeId, ConnectionId, GraphTypeDescription } from '../model';
import { Point } from "@eg/graph";

export enum AppActionTypes {
  LoadApp = '[App] Load App',
  AppLoaded = '[App] App Loaded',
  AppLoadError = '[App] App Load Error',

  MoveNode = "[Workspace] Move Node",
  NodeMoved = "[Workspace] Node Moved",

  AddGraph = "[App] Add Graph",
  GraphAdded = "[App] Graph Added",
  RemoveGraph = "[App] Remove Graph",
  GraphRemoved = "[App] Graph Removed",

  SelectGraph = '[App] Select Graph',

  AddNode = "[Graph] Add Node",
  NodeAdded = "[Graph] Node Added",
  RemoveNode = "[Graph] Remove Node",
  NodeRemoved = "[Graph] Node Removed",

  AddConnection = '[Graph] Add Connection',
  ConnectionAdded = '[Graph] Connection Added',
  RemoveConnection = '[Graph] Remove Connection',
  ConnectionRemoved = '[Graph] Connection Removed',
}

export class LoadApp implements Action {
  readonly type = AppActionTypes.LoadApp;
}

export class AppLoadError implements Action {
  readonly type = AppActionTypes.LoadApp;
  constructor(public error: any) { }
}

export class AppLoaded implements Action {
  readonly type = AppActionTypes.AppLoaded;
  constructor(public graphs: Graph[], public graphDescriptions: GraphTypeDescription[]) { }
}

export class SelectGraph implements Action {
  readonly type = AppActionTypes.SelectGraph;
  constructor(public id: Id) { }
}

// Actions related to the Workspace
// --------------------------------
export class MoveNode implements Action {
  readonly type = AppActionTypes.MoveNode;
  constructor(public node: Node, public newPosition: Point) { }
}
export class NodeMoved implements Action {
  readonly type = AppActionTypes.NodeMoved;
  constructor(public node: Node, public newPosition: Point ) { }
}


// Actions related to GraphManagementService
// -----------------------------------------
export class AddGraph implements Action {
  readonly type = AppActionTypes.AddGraph;
  constructor(public graph: Graph) { }
}

export class GraphAdded implements Action {
  readonly type = AppActionTypes.GraphAdded;
  constructor(public graph: Graph) { }
}

export class RemoveGraph implements Action {
  readonly type = AppActionTypes.RemoveGraph;
  constructor(public id: Id) { }
}

export class GraphRemoved implements Action {
  readonly type = AppActionTypes.GraphRemoved;
  constructor(public id: Id) { }
}

// Actions related to GraphManipulationService
// -------------------------------------------
export class AddNode implements Action {
  readonly type = AppActionTypes.AddNode;
  constructor(public node: Node) { }
}

export class NodeAdded implements Action {
  readonly type = AppActionTypes.NodeAdded;
  constructor(public node: Node) { }
}

export class RemoveNode implements Action {
  readonly type = AppActionTypes.RemoveNode;
  constructor(public id: NodeId) { }
}

export class NodeRemoved implements Action {
  readonly type = AppActionTypes.NodeRemoved;
  constructor(public id: NodeId) { }
}

export class AddConnection implements Action {
  readonly type = AppActionTypes.AddConnection;
  constructor(public source: OutputSocket | InputSocket, public target: OutputSocket | InputSocket) { }
}

export class ConnectionAdded implements Action {
  readonly type = AppActionTypes.ConnectionAdded;
  constructor(public connection: Connection) { }
}

export class RemoveConnection implements Action {
  readonly type = AppActionTypes.RemoveConnection;
  constructor(public connection: Connection) { }
}

export class ConnectionRemoved implements Action {
  readonly type = AppActionTypes.ConnectionRemoved;
  constructor(public connectionId: ConnectionId) { }
}

/**
 *  All Actions for this application.
 */
export type AppAction =
  LoadApp | AppLoaded | AppLoadError | SelectGraph |

  AddGraph | GraphAdded |
  RemoveGraph | GraphRemoved |

  MoveNode | NodeMoved |

  AddNode | NodeAdded |
  RemoveNode | NodeRemoved |
  AddConnection | ConnectionAdded |
  RemoveConnection | ConnectionRemoved;

export const fromAppActions = {
  LoadApp,
  AppLoaded,
  AppLoadError,

  MoveNode, NodeMoved,

  AddGraph, GraphAdded,
  RemoveGraph, GraphRemoved,

  AddNode, NodeAdded,
  RemoveNode, NodeRemoved,
  AddConnection, ConnectionAdded
};
