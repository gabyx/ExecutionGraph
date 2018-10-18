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

import { Component, OnInit, EventEmitter, Output, Input } from '@angular/core';
import {
  ConnectionDrawStyle,
  DirectConnectionDrawStyle,
  ManhattenConnectionDrawStyle,
  BezierConnectionDrawStyle
} from '@eg/graph';
import { ILogger, LoggerFactory } from '@eg/logger';
import { Store } from '@ngrx/store';
import { UiState } from '../../+state/reducers/ui.reducers';
import { SetConnectionDrawStyle } from '../../+state/actions/ui.actions';
import { Observable } from 'rxjs';
import { getConnectionDrawStyleName } from '../../+state/selectors/ui.selectors';

export type ConnectionDrawStyleName = 'direct' | 'manhatten' | 'bezier';

// const drawStyles = {
//   direct: new DirectConnectionDrawStyle(),
//   manhatten: new ManhattenConnectionDrawStyle(),
//   bezier: new BezierConnectionDrawStyle()
// };

@Component({
  selector: 'eg-connection-style-options',
  templateUrl: './connection-style-options.component.html',
  styleUrls: ['./connection-style-options.component.css']
})
export class ConnectionStyleOptionsComponent implements OnInit {

  drawStyle: Observable<ConnectionDrawStyleName>;

  private readonly log: ILogger;

  constructor(private store: Store<UiState>, loggerFactory: LoggerFactory) {
    this.log = loggerFactory.create('ConnectionStyleOptionsComponent');
    this.drawStyle = store.select(getConnectionDrawStyleName);
  }

  ngOnInit() {}

  setDrawStyle(drawStyleName: ConnectionDrawStyleName) {
    this.log.info(`Draw Style changed to ${drawStyleName}`);
    this.store.dispatch(new SetConnectionDrawStyle(drawStyleName));
  }
}
