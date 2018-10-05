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
import { filter, tap } from 'rxjs/operators';

import { LoggerFactory, ILogger } from '@eg/logger';
import * as actions from './+state/actions';
import { GraphsState } from './+state/reducers';
import * as graphQueries from './+state/selectors/graph.selectors';
import { getDrawerRequired } from './+state/selectors';
import { AppState } from './+state/reducers/app.reducers';
import { RouterState, Router } from '@angular/router';

@Component({
  selector: 'eg-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  private readonly log: ILogger;

  public get hasDrawerContent() {
    return this.store.select(getDrawerRequired);
  }

  constructor(private graphStore: Store<GraphsState>, private store: Store<RouterState>, private router: Router, loggerFactory: LoggerFactory) {
    this.log = loggerFactory.create('AppComponent');
  }

  ngOnInit() {
    this.graphStore.dispatch(new actions.LoadGraphDescriptions());
    this.graphStore.dispatch(new actions.LoadGraphs());

    // this.store
    //   .select(graphQueries.getGraphs)
    //   .pipe(filter(graphs => graphs.length > 0))
    //   .subscribe(graphs => {
    //     this.log.debug(`Loaded graphs, auto-selecting first`);
    //     if (graphs.length === 0) {
    //       this.log.error(`Cannot select, since no graphs loaded!`);
    //     } else {
    //       this.store.dispatch(new actions.OpenGraph(graphs[0].id));
    //     }
    //   });
  }

  drawerClosed() {
    this.router.navigate([{outlets: { drawer: null}}]);
  }
}
