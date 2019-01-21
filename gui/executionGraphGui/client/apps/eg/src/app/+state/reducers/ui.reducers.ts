import { ConnectionDrawStyleName } from '../../components/connection-style-options/connection-style-options.component';
import * as fromUiActions from '../actions/ui.actions';
import { NodeId, ConnectionId } from '../../model';

export interface Selection {
  nodes: NodeId[];
  connections: ConnectionId[];
}

export interface UiState {
  connectionDrawStyle: ConnectionDrawStyleName;
  selection: Selection;
}

export const initalState: UiState = {
  connectionDrawStyle: 'bezier',
  selection: {
    nodes: [],
    connections: []
  }
};

export function reducer(state: UiState = initalState, action: fromUiActions.UiAction): UiState {
  switch (action.type) {
    case fromUiActions.SET_CONNECTION_DRAW_STYLE: {
      return {
        ...state,
        connectionDrawStyle: action.drawStyle
      };
    }

    case fromUiActions.SET_SELECTION: {
      return {
        ...state,
        selection: {
          nodes: [...action.nodes],
          connections: [...action.connections]
        }
      };
    }

    case fromUiActions.ADD_SELECTION: {
      return {
        ...state,
        selection: {
          nodes: [...state.selection.nodes, ...action.nodes],
          connections: [...state.selection.connections, ...action.connections]
        }
      };
    }

    case fromUiActions.REMOVE_SELECTION: {
      const nodes = state.selection.nodes.filter(nodeId => action.nodes.indexOf(nodeId) < 0);
      const connections = state.selection.connections.filter(
        connectionId => action.connections.indexOf(connectionId) < 0
      );

      return {
        ...state,
        selection: {
          nodes: [...nodes],
          connections: [...connections]
        }
      };
    }

    case fromUiActions.CLEAR_SELECTION: {
      return {
        ...state,
        selection: {
          nodes: [],
          connections: []
        }
      };
    }

    default:
      return state;
  }
}
