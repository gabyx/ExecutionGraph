import { Action } from '@ngrx/store';
import { Graph } from '../model/Graph';
import { Id } from '@eg/common/src';
import { Socket, Connection, OutputSocket, InputSocket } from '../model';

export enum AppActionTypes {
  LoadApp = '[App] Load App',
  AppLoaded = '[App] App Loaded',
  AppLoadError = '[App] App Load Error',

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
}

export class LoadApp implements Action {
  readonly type = AppActionTypes.LoadApp;
}

export class AppLoadError implements Action {
  readonly type = AppActionTypes.LoadApp;
  constructor(public payload: any) {}
}

export class AppLoaded implements Action {
  readonly type = AppActionTypes.AppLoaded;
  constructor(public payload: Graph[]) {}
}

export class SelectGraph implements Action {
  readonly type = AppActionTypes.SelectGraph;
  constructor(public id: Id) { }
}

// Actions related to GraphManagementService
//------------------------------------------
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
//--------------------------------------------
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
  constructor(public id: Id) { }
}

export class NodeRemoved implements Action {
  readonly type = AppActionTypes.NodeRemoved;
  constructor(public id: Id) { }
}

export class AddConnection implements Action {
  readonly type = AppActionTypes.AddConnection;
  constructor(public source: OutputSocket | InputSocket, public target: OutputSocket | InputSocket) { }
}

export class ConnectionAdded implements Action {
  readonly type = AppActionTypes.ConnectionAdded;
  constructor(public connection: Connection) { }
}

/**
 *  All Actions for this application.
 */
export type AppAction = LoadApp | AppLoaded | AppLoadError | SelectGraph | AddConnection | ConnectionAdded;

export const fromAppActions = {
  LoadApp,
  AppLoaded,
  AppLoadError,
  RemoveGraph,
  AddConnection,
  ConnectionAdded
};
