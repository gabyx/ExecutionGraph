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

import { ExecutionService } from '../../services/ExecutionService';

@Component({
  selector: 'eg-toolbar',
  templateUrl: './toolbar.component.html',
  styleUrls: ['./toolbar.component.scss']
})
export class ToolbarComponent implements OnInit {
  public testResponse: any;

  constructor(private readonly executionService: ExecutionService) {}

  ngOnInit() {}

  public test() {
    console.log('Testing');
    this.executionService.execute().then(
      result => {
        this.testResponse = result;
      },
      error => {
        this.testResponse = error;
      }
    );
  }
}
