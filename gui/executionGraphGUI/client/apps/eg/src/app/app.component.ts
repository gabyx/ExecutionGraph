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
import { Observable } from 'rxjs';
import { LoggerFactory, ILogger } from '@eg/logger';
import { Id, isDefined } from '@eg/common';
import { AppState } from './+state/app.state';
import { fromAppActions } from './+state/app.actions';
import { appQuery } from './+state/app.selectors';
import { UIPropertiesInspector } from './+state/app.uiproperties';

@Component({
  selector: 'eg-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  private readonly log: ILogger;

  public inspectorProps: Observable<UIPropertiesInspector>;

  constructor(private store: Store<AppState>, loggerFactory: LoggerFactory) {
    this.log = loggerFactory.create('AppComponent');
  }

  ngOnInit() {
    this.inspectorProps = this.store
      .select(appQuery.getInspectorUIProperties)
      .pipe(filter(uiProps => isDefined(uiProps)));

    this.inspectorProps.subscribe((props: UIPropertiesInspector) => {
      this.log.debug('AppComponent:: inspectorProps changed');
    });

    this.store.dispatch(new fromAppActions.LoadApp());

    this.store
      .select(appQuery.getAllGraphs)
      .pipe(filter(graph => isDefined(graph)))
      .subscribe(graphs => {
        this.log.debug(`Loaded graphs, auto-selecting first`);
        if (graphs.size === 0) {
          this.log.error(`Cannot select, since no graphs loaded!`);
        } else {
          this.store.dispatch(new fromAppActions.SelectGraph(graphs.keys().next().value));
        }
      });
  }
}
