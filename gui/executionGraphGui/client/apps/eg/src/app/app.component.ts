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
import { RouterState, Router } from '@angular/router';
import { Store, select } from '@ngrx/store';
import { filter, tap, take } from 'rxjs/operators';

import { LoggerFactory, ILogger } from '@eg/logger';
import * as actions from './+state/actions';
import { GraphsState } from './+state/reducers';
import * as graphQueries from './+state/selectors/graph.selectors';
import { getDrawerRequired } from './+state/selectors';

@Component({
  selector: 'eg-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  private readonly log: ILogger;

  public get hasDrawerContent() {
    return this.store.pipe(select(getDrawerRequired));
  }

  constructor(
    private graphStore: Store<GraphsState>,
    private store: Store<RouterState>,
    private router: Router,
    loggerFactory: LoggerFactory
  ) {
    this.log = loggerFactory.create('AppComponent');
  }

  ngOnInit() {
    this.graphStore.dispatch(new actions.LoadGraphDescriptions());
    this.graphStore.dispatch(new actions.LoadGraphs());

    this.store
      .pipe(
        select(graphQueries.getGraphs),
        filter(graphs => graphs.length > 0),
        tap(graphs => {
          this.log.debug(`Loaded graphs, auto-selecting first`);
          if (graphs.length === 0) {
            this.log.error(`Cannot select, since no graphs loaded!`);
          } else {
            this.router.navigate([
              {
                outlets: {
                  primary: ['graph', graphs[0].id],
                  drawer: ['nodes']
                }
              }
            ]);

            this.graphStore.dispatch(
              new actions.ShowNotification(`To help you debug I created a dummy graph for you \u{1F64C}`, 7000)
            );
          }
        }),
        take(1)
      )
      .subscribe(graphs => {});
  }

  drawerClosed() {
    this.router.navigate([{ outlets: { drawer: null } }]);
  }
}
