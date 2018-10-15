import { createSelector } from "@ngrx/store";
import { getUiState } from "./app.selectors";
import { BezierConnectionDrawStyle, DirectConnectionDrawStyle, ManhattenConnectionDrawStyle } from "@eg/graph";

export const getConnectionDrawStyleName = createSelector(
  getUiState,
  (state) => state.connectionDrawStyle
);

export const getConnectionDrawStyle = createSelector(
  getConnectionDrawStyleName,
  drawStyleName => {
    switch(drawStyleName) {
      case 'direct': {
        return new DirectConnectionDrawStyle();
      }
      case 'manhatten': {
        return new ManhattenConnectionDrawStyle();
      }
      case 'bezier': {
        return new BezierConnectionDrawStyle();
      }
      default: {
        throw new Error(`Invalid connection draw style name ${drawStyleName}`);
      }
    }
  }
);

export const getSelection = createSelector(
  getUiState,
  (state) => state.selection
);