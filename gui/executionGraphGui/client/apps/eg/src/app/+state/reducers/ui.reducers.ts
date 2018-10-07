import { ConnectionDrawStyleName } from "../../components/connection-style-options/connection-style-options.component";
import { UiAction, UiActions } from "../actions/ui.actions";

export interface UiState {
  connectionDrawStyle: ConnectionDrawStyleName
}

export const initalState: UiState = {
  connectionDrawStyle: 'bezier'
};

export function reducer(state: UiState = initalState, action: UiAction): UiState {

  switch(action.type) {

    case UiActions.SET_CONNECTION_DRAW_STYLE: {
      state = {...state, connectionDrawStyle: action.drawStyle};
    }

    default:
      return {...state};
  }
}
