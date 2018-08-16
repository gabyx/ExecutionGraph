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

type ConnectionDrawStyleName = 'direct' | 'manhatten' | 'bezier';

const drawStyles = {
  direct: new DirectConnectionDrawStyle(),
  manhatten: new ManhattenConnectionDrawStyle(),
  bezier: new BezierConnectionDrawStyle()
};

@Component({
  selector: 'eg-connection-style-options',
  templateUrl: './connection-style-options.component.html',
  styleUrls: ['./connection-style-options.component.css']
})
export class ConnectionStyleOptionsComponent implements OnInit {
  private drawStyleName: ConnectionDrawStyleName;

  @Output() connectionDrawStyleChanged = new EventEmitter<ConnectionDrawStyle>();
  @Input() connectionDrawStyle: ConnectionDrawStyle = drawStyles['direct'];

  get drawStyle(): ConnectionDrawStyleName {
    return this.drawStyleName;
  }
  set drawStyle(drawStyleName: ConnectionDrawStyleName) {
    this.drawStyleName = drawStyleName;
    this.connectionDrawStyle = drawStyles[this.drawStyleName];
    console.log(`[ConnectionStyleOptionsComponent] Draw Style changed to ${this.drawStyleName}`);
    this.connectionDrawStyleChanged.emit(this.connectionDrawStyle);
  }

  constructor() {
    this.drawStyle = 'direct';
  }

  ngOnInit() {}
}
