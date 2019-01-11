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
import { GeneralInfoService, sz } from './GeneralInfoService';
import { ITestBackend } from './TestBackend';
import { ILogger, LoggerFactory } from '@eg/logger';
import * as model from '../model';

@Injectable()
export class GeneralInfoServiceDummy extends GeneralInfoService {
  private logger: ILogger;

  constructor(
    loggerFactory: LoggerFactory,
    private backend: ITestBackend,
    @Inject(VERBOSE_LOG_TOKEN) private readonly verboseResponseLog = true
  ) {
    super();
    this.logger = loggerFactory.create('GeneralInfoServiceDummy');
  }

  public async getAllGraphTypeDescriptions(): Promise<model.GraphTypeDescription[]> {
    const graphDescs = this.backend.graphTypeDescs;

    if (this.verboseResponseLog) {
      this.logger.debug(`GraphDescriptions: ${JSON.stringify(graphDescs)}`, null, 4);
    }
    return graphDescs;
  }
}
