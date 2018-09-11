import { Action } from '@ngrx/store';
import { Graph } from '../model/Graph';
import { Id } from '@eg/common/src';
import { Socket, Connection, OutputSocket, InputSocket } from '../model';

export enum AppActionTypes {
  LoadApp = '[App] Load App',
  AppLoaded = '[App] App Loaded',
  AppLoadError = '[App] App Load Error',
  SelectGraph = '[Graph] Select Graph',
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

export class AddConnection implements Action {
  readonly type = AppActionTypes.AddConnection;
  constructor(public source: OutputSocket | InputSocket, public target: OutputSocket | InputSocket) { }
}

export class ConnectionAdded implements Action {
  readonly type = AppActionTypes.ConnectionAdded;
  constructor(public connection: Connection) { }
}

export type AppAction = LoadApp | AppLoaded | AppLoadError | SelectGraph | AddConnection | ConnectionAdded;

export const fromAppActions = {
  LoadApp,
  AppLoaded,
  AppLoadError,
  SelectGraph,
  AddConnection,
  ConnectionAdded
};
