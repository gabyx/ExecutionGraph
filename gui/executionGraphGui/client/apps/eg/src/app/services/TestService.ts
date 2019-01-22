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
import { ILogger, LoggerFactory } from '@eg/logger';
import { GeneralInfoService } from './GeneralInfoService';
import { GraphManipulationService } from './GraphManipulationService';
import { GraphManagementService } from './GraphManagementService';
import { FileBrowserService } from './FileBrowserService';
import * as Long from 'long';

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
    private readonly graphManagementService: GraphManagementService,
    private readonly fileBrowserService: FileBrowserService
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
    this.logger.debug('Get all graph type descriptions...');
    const graphDescs = await this.generalInfoService.getAllGraphTypeDescriptions();

    // Add a node to the first graph
    const graphDesc = graphDescs[0];
    const graphTypeId = graphDesc.id;
    const nodeType = graphDesc.nodeTypeDescriptions[0].type;

    // Add a graph
    const graph = await this.graphManagementService.addGraph(graphTypeId);

    // Add the node.
    await this.graphManipulationService.addNode(graph.id, nodeType, 'MySuperDuperNode');

    // Add a non existing node
    try {
      await this.graphManipulationService.addNode(
        graph.id,
        'BananaNode',
        'MySupercalifragilisticexpialidociousBananaNode'
      );
    } catch (error) {
      this.logger.error(error);
    }

    // Remove first node
    await this.graphManipulationService.removeNode(graph.id, Long.fromNumber(0));

    // Remove graph
    await this.graphManagementService.removeGraph(graph.id);
  }

  public async testFileBrowser() {
    await this.fileBrowserService.browse('./');
  }
}
