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

import { Component, OnInit, Injectable } from '@angular/core';
import { Observable } from 'rxjs';

import { Graph } from '../../model';
import { Store } from '@ngrx/store';
import { GraphsState } from '../../+state/reducers';
import { getGraphs } from '../../+state/selectors';

@Injectable()
@Component({
  selector: 'eg-toolbar',
  templateUrl: './toolbar.component.html',
  styleUrls: ['./toolbar.component.scss']
})
export class ToolbarComponent implements OnInit {

  public graphsMRU: Observable<Graph[]>;

  constructor(private store: Store<GraphsState>) {
    this.graphsMRU = store.select(getGraphs);
  }

  ngOnInit() {}
}
