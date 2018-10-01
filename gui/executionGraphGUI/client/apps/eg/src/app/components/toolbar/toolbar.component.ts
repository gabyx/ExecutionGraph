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
import { Store } from '@ngrx/store';
import { AppState, fromAppActions } from '../../+state';
import { appQuery } from '../../+state/app.selectors';
import { TestService } from '../../services/TestService';

@Injectable()
@Component({
  selector: 'eg-toolbar',
  templateUrl: './toolbar.component.html',
  styleUrls: ['./toolbar.component.scss']
})
export class ToolbarComponent implements OnInit {
  public testResponse: any;

  constructor(private store: Store<AppState>, private readonly testService: TestService) {}

  ngOnInit() {}

  public showInspector() {
    this.store.dispatch(new fromAppActions.UpdateUIProperties({ inspector: { isVisible: true } }));
  }

  public test() {
    this.testService.testAddRemove();
  }
}
