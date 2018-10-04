// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jul 29 2018
//  @author Simon Spoerri, simon (døt) spoerri (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { Component, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { filter } from 'rxjs/operators';

import { LoggerFactory, ILogger } from '@eg/logger';
import * as graphActions from './+state/actions';
import { GraphsState } from './+state/reducers';
import * as graphQueries from './+state/selectors/graph.selectors';

@Component({
  selector: 'eg-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  private readonly log: ILogger;

  constructor(private store: Store<GraphsState>, loggerFactory: LoggerFactory) {
    this.log = loggerFactory.create('AppComponent');
  }

  ngOnInit() {
    this.store.dispatch(new graphActions.LoadGraphs());

    this.store
      .select(graphQueries.getGraphEntities)
      .pipe(filter(entities => entities.size > 0))
      .subscribe(entities => {
        this.log.debug(`Loaded graphs, auto-selecting first`);
        if (entities.size === 0) {
          this.log.error(`Cannot select, since no graphs loaded!`);
        } else {
          this.store.dispatch(new graphActions.SelectGraph(entities.keys().next().value));
        }
      });
  }
}
