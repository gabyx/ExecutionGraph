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

import { Component, Input } from "@angular/core";

import { ConnectionDrawStyle } from "../../model/ConnectionDrawStyle";
import { BezierConnectionDrawStyle } from "../../model/BezierConnectionDrawStyle";

@Component({
  selector: "ngcs-connection",
  templateUrl: "./connection.component.html",
  styleUrls: ["./connection.component.scss"]
})
export class ConnectionComponent {
  @Input() from: string;

  @Input() to: string;

  @Input() drawStyle: ConnectionDrawStyle = new BezierConnectionDrawStyle();
}
