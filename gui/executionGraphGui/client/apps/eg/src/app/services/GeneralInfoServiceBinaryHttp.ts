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

import { Injectable, Inject } from '@angular/core';
import { VERBOSE_LOG_TOKEN } from '../tokens';
import { flatbuffers } from 'flatbuffers';
import { ILogger, LoggerFactory } from '@eg/logger';
import { GeneralInfoService, sz } from './GeneralInfoService';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';
import * as model from '../model';
import * as conversions from './Conversions';

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

  public async getAllGraphTypeDescriptions(): Promise<model.GraphTypeDescription[]> {
    const result = await this.binaryRouter.get('general/getAllGraphTypeDescriptions');
    const buf = new flatbuffers.ByteBuffer(result);
    const response = sz.GetAllGraphTypeDescriptionsResponse.getRoot(buf);

    this.logger.info(`Number of graph types: ${response.graphsTypesLength()}`);

    const graphDesc: model.GraphTypeDescription[] = [];

    for (let g = 0; g < response.graphsTypesLength(); ++g) {
      graphDesc.push(conversions.toGraphTypeDescription(response.graphsTypes(g)));
    }

    if (this.verboseResponseLog) {
      this.logger.debug(`GraphDescriptions: ${JSON.stringify(graphDesc, null, 4)}`);
    }

    return graphDesc;
  }
}
