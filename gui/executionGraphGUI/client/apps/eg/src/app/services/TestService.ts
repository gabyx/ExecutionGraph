// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Fri Aug 17 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { Injectable } from '@angular/core';
import { flatbuffers } from 'flatbuffers';
import { ILogger, LoggerFactory } from '@eg/logger';
import { GeneralInfoService } from './GeneralInfoService';
import { GraphManipulationService } from './GraphManipulationService';
import { GraphManagementService } from './GraphManagementService';
import { NodeId } from './../model';

/**
 * Stupid TestService class which can be quickly injected
 * for testing pruposes into components which shouldnt have these
 * dependencies anyway.
 *
 * @export
 * @class TestService
 */
@Injectable()
export class TestService {
  private logger: ILogger;

  constructor(
    loggerFactory: LoggerFactory,
    private readonly generalInfoService: GeneralInfoService,
    private readonly graphManipulationService: GraphManipulationService,
    private readonly graphManagementService: GraphManagementService
  ) {
    this.logger = loggerFactory.create('TestService');
  }

  /**
   * Test function to test add/remove graph and nodes.
   *
   * @memberof TestService
   */
  public async testAddRemove() {
    // Get the graph infos
    console.debug('Get all graph type descriptions...');
    let graphDescs = await this.generalInfoService.getAllGraphTypeDescriptions();

    // Add a node to the first graph
    let graphDesc = graphDescs[0];
    let graphTypeId = graphDesc.id;
    let nodeType = graphDesc.nodeTypeDescritptions[0].type;

    // Add a graph
    let graphId = await this.graphManagementService.addGraph(graphTypeId);

    // Add the node.
    await this.graphManipulationService.addNode(graphId, nodeType, 'MySuperDuperNode');

    // Add a non existing node
    await this.graphManipulationService.addNode(
      graphId,
      'BananaNode',
      'MySupercalifragilisticexpialidociousBananaNode'
    );

    // Remove first node
    await this.graphManipulationService.removeNode(graphId, new NodeId(0));
  }
}
