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
import { LoggerFactory } from '@eg/logger/src';

export enum LayoutStrategys {
  MassSpringSystem = 0,
  MassSpringSystemSimon = 1
}

export abstract class ILayoutStrategy {
  public readonly strategy: LayoutStrategys;

  //! Injecting ourself into the engine such that
  //! the user does need to know how to instanciate the engine.
  //! @todo This function is ok to be here (imho), but would be nice to
  //! find a solution with the DI system of angular.
  public abstract createEngine(loggerFactory: LoggerFactory): ILayoutEngine;
}
