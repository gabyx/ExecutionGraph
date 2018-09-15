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
import { Id } from "@eg/common"
import { AppState } from './+state/AppState';
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

    this.store.select(appQuery.getAllGraphs)
      .subscribe(graphs => {
        let defaultId = "644020cc-1f8b-4e50-9210-34f4bf2308d4";
        this.log.debug(`Loaded graphs, auto-selecting ${defaultId}`);
        if (!(defaultId in graphs)) {
          this.log.error(`Could not select graph id ${defaultId}!`)
        }
        else
        {
          this.store.dispatch(new SelectGraph(new Id(defaultId)))
        }
      });
  }
}
