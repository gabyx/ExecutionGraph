import { Action } from "@ngrx/store";
import { ConnectionDrawStyleName } from "../../components/connection-style-options/connection-style-options.component";

export const UiActions = {
  SET_CONNECTION_DRAW_STYLE: "[UI] Set connection Draw Style"
};

export class SetConnectionDrawStyle implements Action {
  readonly type = UiActions.SET_CONNECTION_DRAW_STYLE;

  constructor(public readonly drawStyle: ConnectionDrawStyleName) {
  }
}

export type UiAction = SetConnectionDrawStyle;
