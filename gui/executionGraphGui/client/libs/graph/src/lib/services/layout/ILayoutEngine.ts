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
import { Point } from '../../model/Point';
import { ILayoutStrategy, LayoutStrategys } from './ILayoutStrategy';

export interface Node {
  readonly position: Point;
}

export interface Edge {
  readonly from: Node;
  readonly to: Node;
}

export interface Graph {
  readonly nodes: Node[];
  readonly edges: Edge[];
}

export abstract class ILayoutEngine {
  public async dispatchRun(config: ILayoutStrategy, graph: Graph): Promise<void> {
    if (this.isStrategyValid(config.strategy)) {
      return this.run(config, graph);
    }
    throw `Wrong strategy '${config.strategy}' for engine!`;
  }

  public abstract run(config: ILayoutStrategy, graph: Graph): Promise<void>;
  public abstract isStrategyValid(strategy: LayoutStrategys): boolean;
}
