// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jan 13 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { ILayoutEngine } from './ILayoutEngine';
import { LoggerFactory, ILogger } from '@eg/logger/src';
import { Injectable } from '@angular/core';
import { LayoutStrategys, ILayoutStrategy } from './ILayoutStrategy';
import { Graph } from './ILayoutEngine';

export class MassSpringLayoutStrategy extends ILayoutStrategy {
  public static readonly strategy_ = LayoutStrategys.MassSpringSystem;
  public readonly strategy = MassSpringLayoutStrategy.strategy_;

  massRange: [number, number] = [1, 10];
  timeStep: number = 0.1;

  convergeRelTol: number = 1e-3;
  convergeAbsTol: number = 1e-3;

  public createEngine(loggerFactory: LoggerFactory): ILayoutEngine {
    return new MassSpringLayoutEngine(loggerFactory);
  }
}

@Injectable()
export class MassSpringLayoutEngine extends ILayoutEngine {
  private readonly logger: ILogger;

  constructor(private readonly loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('MassSpringLayoutEngine');
  }

  public isStrategyValid(s: LayoutStrategys) {
    return s === MassSpringLayoutStrategy.strategy_;
  }

  async run(config: MassSpringLayoutStrategy, graph: Graph): Promise<void> {}
}
