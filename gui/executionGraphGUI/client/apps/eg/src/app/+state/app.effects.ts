import { Injectable } from '@angular/core';
import { from } from "rxjs"
import { Effect, Actions } from '@ngrx/effects';
import { DataPersistence } from '@nrwl/nx';
import { Id } from '@eg/common';
import { LoggerFactory, ILogger } from '@eg/logger';
import { LoadApp, AppLoaded, AppLoadError, AppActionTypes, ConnectionAdded, AddConnection } from './app.actions';
import { createConnection } from '../model';
import { AppState } from './app.state';
import * as services from '../services';

@Injectable()
export class AppEffects {
  private readonly log: ILogger;

  private async loadApp(): Promise<AppLoaded> {
    // Get Graph Infos
    const graphDescs = await this.generalInfoService.getAllGraphTypeDescriptions();
    let graphDesc = graphDescs[0];
    let graphTypeId = graphDesc.id;
    let nodeType = graphDesc.nodeTypeDescritptions[0].type;

    // Add a graph
    let graph = await this.graphManagementService.addGraph(graphTypeId);

    // Add nodes.
    for(let i = 0; i < 3 ; ++i){
      let node = await this.graphManipulationService.addNode(graph.id, nodeType, `${nodeType}-${i}`);
      graph.nodes.push(node);
    }
    return new AppLoaded([graph], graphDescs);
}

@Effect()
loadApp$ = this.dataPersistence.fetch(AppActionTypes.LoadApp, {
  run: (action: LoadApp, state: AppState) => {
    return from(this.loadApp());
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
  private dataPersistence: DataPersistence < AppState >,
  loggerFactory: LoggerFactory,
  private readonly generalInfoService: services.GeneralInfoService,
  private readonly graphManipulationService: services.GraphManipulationService,
  private readonly graphManagementService: services.GraphManagementService
) {
  this.log = loggerFactory.create('AppEffects');
}
}
