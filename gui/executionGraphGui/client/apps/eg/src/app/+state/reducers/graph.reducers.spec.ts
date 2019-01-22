import * as fromActions from '../actions/graph.actions';
import { reducer, initialState } from './graph.reducers';

describe('Graph Reducer', () => {
  describe('unknown action', () => {
    it('should return the initial state', () => {
      const action = {} as any;
      const result = reducer(initialState, action);

      expect(result).toBe(initialState);
    });
  });

  describe('GraphsLoaded', () => {
    it('should set the loaded flag to true', () => {
      const result = reducer(initialState, new fromActions.GraphsLoaded([]));

      expect(result.loaded).toBeTruthy();
    });
  });
});
