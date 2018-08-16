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

import { Injectable, Inject, Testability } from '@angular/core';
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/first';

import { flatbuffers } from 'flatbuffers';
import { GeneralInfoService, sz } from './GeneralInfoService';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';
import { ILogger, LoggerFactory } from '@eg/logger';
import { VERBOSE_LOG_TOKEN } from '../tokens';

@Injectable()
export class GeneralInfoServiceBinaryHttp extends GeneralInfoService {
  private logger: ILogger;

  constructor(
    loggerFactory: LoggerFactory,
    private readonly binaryRouter: BinaryHttpRouterService,
    @Inject(VERBOSE_LOG_TOKEN) private readonly verboseResponseLog = true
  ) {
    super();
    this.logger = loggerFactory.create('GeneralInfoServiceBinaryHttp');
  }

  public async getAllGraphTypeDescriptions(): Promise<sz.GetAllGraphTypeDescriptionsResponse> {
    const result = await this.binaryRouter.get('general/getAllGraphTypeDescriptions');
    let buf = new flatbuffers.ByteBuffer(result);
    let response = sz.GetAllGraphTypeDescriptionsResponse.getRootAsGetAllGraphTypeDescriptionsResponse(buf);
    this.logger.info(`Received: Number of Graph types: ${response.graphsTypesLength()}`);

    // Verbose logging the response if enabled
    if (this.verboseResponseLog) {
      for (let g = 0; g < response.graphsTypesLength(); ++g) {
        let graphDesc = response.graphsTypes(g);
        this.logger.info(`Infos for graph '${graphDesc.name()}' with id '${graphDesc.id()}' :`);
        this.logger.info('Sockets:');
        for (let i = 0; i < graphDesc.socketTypeDescriptionsLength(); ++i) {
          let socketDesc = graphDesc.socketTypeDescriptions(i);
          this.logger.info(`Socket: ${socketDesc.name()} [${socketDesc.type()}]`);
        }
        this.logger.info('Nodes:');
        for (let i = 0; i < graphDesc.nodeTypeDescriptionsLength(); ++i) {
          let nodeDesc = graphDesc.nodeTypeDescriptions(i);
          this.logger.info(`Node: ${nodeDesc.name()} [${nodeDesc.type()}]`);
        }
      }
    }

    return response;
  }
}
