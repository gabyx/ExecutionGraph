import * as fromUiActions from '../actions/ui.actions';
import { initalState, reducer } from './ui.reducers';
import { NodeId, ConnectionId } from '../../model';

describe('UI Reducer', () => {
  describe('unknown action', () => {
    it('should return the initial state', () => {
      const action = {} as any;
      const result = reducer(initalState, action);

      expect(result).toBe(initalState);
    });
  });

  describe('AddSelection', () => {
    it('should add to the existing selection', () => {
      const result = reducer(initalState, new fromUiActions.AddSelection([new NodeId(0)], ['a']));

      expect(result.selection.nodes.length).toBe(1);
      expect(result.selection.connections.length).toBe(1);

      const result1 = reducer(result, new fromUiActions.AddSelection([new NodeId(1)], ['b']));

      expect(result1.selection.nodes.length).toBe(2);
      expect(result1.selection.connections.length).toBe(2);
    });
  });

  describe('RemoveSelection', () => {
    it('should remove from the existing selection', () => {
      const nodeId = new NodeId(0);

      const result = reducer(initalState, new fromUiActions.AddSelection([nodeId, new NodeId(1)], ['a']));

      expect(result.selection.nodes.length).toBe(2);
      expect(result.selection.connections.length).toBe(1);

      const result1 = reducer(result, new fromUiActions.RemoveSelection([nodeId], []));

      expect(result1.selection.nodes.length).toBe(1);
      expect(result1.selection.connections.length).toBe(1);
    });
  });

  describe('SetSelection', () => {
    it('should set to the exact selection', () => {
      const result = reducer(initalState, new fromUiActions.AddSelection([new NodeId(1)], ['a']));

      expect(result.selection.nodes.length).toBe(1);
      expect(result.selection.connections.length).toBe(1);

      const result1 = reducer(result, new fromUiActions.SetSelection([new NodeId(2), new NodeId(3)], []));

      expect(result1.selection.nodes.length).toBe(2);
      expect(result1.selection.connections.length).toBe(0);
    });
  });

  describe('ClearSelection', () => {
    it('should clear the selection', () => {
      const result = reducer(initalState, new fromUiActions.AddSelection([new NodeId(1)], ['a']));

      expect(result.selection.nodes.length).toBe(1);
      expect(result.selection.connections.length).toBe(1);

      const result1 = reducer(result, new fromUiActions.ClearSelection());

      expect(result1.selection.nodes.length).toBe(0);
      expect(result1.selection.connections.length).toBe(0);
    });
  });
});
