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
import { GraphManipulationService } from '../../services/GraphManipulationService';
import { GeneralInfoService } from '../../services/GeneralInfoService';

@Component({
  selector: 'eg-toolbar',
  templateUrl: './toolbar.component.html',
  styleUrls: ['./toolbar.component.scss']
})
export class ToolbarComponent implements OnInit {
  public testResponse: any;

  constructor(
    private readonly executionService: ExecutionService,
    private readonly generalInfoService: GeneralInfoService,
    private readonly graphManipulationService: GraphManipulationService) { }

  ngOnInit() { }

  public test() {
    console.log('Testing AddNode');
    this.debugAddNode();
  }

  /**
   * Add some node for debugging purposes
   */
  private async debugAddNode() {
    // Get the graph infos
    console.debug("Get all graph type descriptions...")
    var graphInfos = await this.generalInfoService.getAllGraphTypeDescriptions();

    // Add a node to the first graph
    let graphInfo = graphInfos.graphsTypes(0);
    let graphId = graphInfo.id();
    let nodeType = graphInfo.nodeTypeDescriptions(0).type();

    // Add the node.
    this.graphManipulationService.addNode(graphId, nodeType, "MySuperDuperNode");
  }

}
