import { Action } from "@ngrx/store";
import { ConnectionDrawStyleName } from "../../components/connection-style-options/connection-style-options.component";
import { ConnectionId, NodeId } from "../../model";

export const SET_CONNECTION_DRAW_STYLE = "[UI] Set connection Draw Style"
export const CLEAR_SELECTION = "[UI] Clear Selection"
export const SET_SELECTION = "[UI] Set Selection"
export const ADD_SELECTION = "[UI] Add Selection"
export const REMOVE_SELECTION = "[UI] Remove Selection"

export class SetConnectionDrawStyle implements Action {
  readonly type = SET_CONNECTION_DRAW_STYLE;

  constructor(public readonly drawStyle: ConnectionDrawStyleName) {
  }
}
export class ClearSelection implements Action {
  readonly type = CLEAR_SELECTION;
  constructor() { }
}
export class SetSelection implements Action {
  readonly type = SET_SELECTION;
  constructor(public nodes: NodeId[] = [], public connections: ConnectionId[] = []) { }
}
export class AddSelection implements Action {
  readonly type = ADD_SELECTION;
  constructor(public nodes: NodeId[] = [], public connections: ConnectionId[] = []) { }
}
export class RemoveSelection implements Action {
  readonly type = REMOVE_SELECTION;
  constructor(public nodes: NodeId[] = [], public connections: ConnectionId[] = []) { }
}

export type UiAction =
  | SetConnectionDrawStyle
  | SetSelection
  | AddSelection
  | RemoveSelection
  | ClearSelection;
