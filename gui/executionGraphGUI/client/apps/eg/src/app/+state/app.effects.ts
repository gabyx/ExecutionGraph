import { Injectable } from '@angular/core';
import { Effect, Actions } from '@ngrx/effects';
import { DataPersistence } from '@nrwl/nx';

import { Id } from '@eg/common';
import { LoggerFactory, ILogger } from '@eg/logger';
import { AppState } from './app.reducer';
import { LoadApp, AppLoaded, AppLoadError, AppActionTypes, ConnectionAdded, AddConnection } from './app.actions';
import { Graph, InputSocket, SocketIndex, Node, NodeId, OutputSocket, createConnection } from '../model';

@Injectable()
export class AppEffects {
  private readonly log: ILogger;

  @Effect()
  loadApp$ = this.dataPersistence.fetch(AppActionTypes.LoadApp, {
    run: (action: LoadApp, state: AppState) => {
      // Your custom REST 'load' logic goes here. For now just return an empty list...
      const n1 = new Node(
        new NodeId(0),
        'Add',
        'Eis und eis git drüü',
        [
          new InputSocket('float', 'Lustiger Pete', new SocketIndex(0)),
          new InputSocket('float', 'Listiger Luch', new SocketIndex(1))
        ],
        [new OutputSocket('float', 'Garstiger Hans', new SocketIndex(0))]);

      const n2 = new Node(
        new NodeId(1),
        'Const',
        'Foif',
        [],
        [new OutputSocket('float', 'Dä Foifer', new SocketIndex(0))]);

      const n3 = new Node(
        new NodeId(2),
        'Const',
        'Sächs',
        [],
        [new OutputSocket('float', 'Dä Sächser', new SocketIndex(0))]);

      const g = new Graph(new Id(), [n1, n2, n3], []);

      return new AppLoaded([
        g
      ]);
    },

    onError: (action: LoadApp, error) => {
      this.log.error('Error', error);
      return new AppLoadError(error);
    }
  });

  @Effect()
  addConnection$ = this.dataPersistence.pessimisticUpdate(AppActionTypes.AddConnection, {
    run: (action: AddConnection, state: AppState) => {
      return new ConnectionAdded(createConnection(action.source, action.target));
    },

    onError: (action: AddConnection, error) => {
      this.log.error('Error', error);
      return new AppLoadError(error);
    }
  });

  constructor(
    private actions$: Actions,
    private dataPersistence: DataPersistence<AppState>,
    loggerFactory: LoggerFactory
  ) {
    this.log = loggerFactory.create('AppEffects');
  }
}
