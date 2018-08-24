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

import { AppState } from './+state/app.reducer';
import { LoadApp, SelectGraph } from './+state/app.actions';
import { appQuery } from './+state/app.selectors';

@Component({
  selector: 'eg-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  
  private readonly log: ILogger;

  constructor(private store: Store<AppState>, loggerFactory: LoggerFactory) {
    this.log = loggerFactory.create("AppComponent");
  }

  ngOnInit() {
    this.store.dispatch(new LoadApp());

    this.store.select(appQuery.getAllApp)
      .pipe(filter(graphs => graphs.length > 0))
      .subscribe(graphs => {
        this.log.debug(`Loaded graphs, auto-selecting the first one`);
        this.store.dispatch(new SelectGraph(graphs[0].id))
      });
  }
}
