import { Injectable } from '@angular/core';
import { Effect, Actions } from '@ngrx/effects';
import { DataPersistence } from '@nrwl/nx';
import { Id } from '@eg/common';
import { LoggerFactory, ILogger } from '@eg/logger';
import { LoadApp, AppLoaded, AppLoadError, AppActionTypes, ConnectionAdded, AddConnection } from './app.actions';
import {
  Graph, GraphTypeDescription,
  InputSocket, SocketIndex, Node, NodeId, OutputSocket, createConnection
} from '../model';
import { AppState } from './app.state';
import * as services from '../services';

@Injectable()
export class AppEffects {
  private readonly log: ILogger;

  @Effect()
  loadApp$ = this.dataPersistence.fetch(AppActionTypes.LoadApp, {
    run: (action: LoadApp, state: AppState) => {

      // // Get Graph Infos
      // generalInfoService.getAllTypeDesc

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

      const g = new Graph(new Id("644020cc-1f8b-4e50-9210-34f4bf2308d4"), new Id(), [n1, n2, n3], []);
      return new AppLoaded([g], []);
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
    loggerFactory: LoggerFactory,
    private readonly generalInfoService: services.GeneralInfoService,
    private readonly graphManipulationService: services.GraphManipulationService,
    private readonly graphManagementService: services.GraphManagementService
  ) {
    this.log = loggerFactory.create('AppEffects');
  }
}
