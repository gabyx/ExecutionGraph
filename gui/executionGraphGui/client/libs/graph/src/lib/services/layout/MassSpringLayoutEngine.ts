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
import { Point, Position } from '../../model/Point';
import { ILayoutEngine, GraphConverter, EngineOutput } from './ILayoutEngine';
import { LoggerFactory, ILogger } from '@eg/logger/src';
import { Injectable } from '@angular/core';
import { LayoutStrategys, ILayoutStrategy } from './ILayoutStrategy';
import { NodeId } from 'apps/eg/src/app/model';

type Force = Point;

abstract class ForceLaw {
  public force: Force = Point.zero.copy();
  public abstract calculateForce(b1: Body, b2: Body): void;
}

class SpringDamper extends ForceLaw {
  public calculateForce(b1: Body, b2: Body) {
    this.force.xy = [1, 0];
  }
}

class Body {
  constructor(public id: NodeId, public position: Position) {}
  public mass: number = 1.0;
  public velocity = Point.one.copy();
  public oldPosition = Point.one.copy();
  public geometry: string;
  public forceLaws: ForceLaw[] = [];
}

class Link {
  constructor(public readonly b1: Body, public readonly b2: Body) {}
  public forceLaws: ForceLaw[] = [];
}

type BodyMap = Map<NodeId, Body>;

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

  private bodies: BodyMap;
  private links: Link[];
  private forceLaws: ForceLaw[];

  constructor(private readonly loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('MassSpringLayoutEngine');
  }

  public isStrategyValid(s: LayoutStrategys) {
    return s === MassSpringLayoutStrategy.strategy_;
  }

  async run(config: MassSpringLayoutStrategy, converter: GraphConverter): Promise<EngineOutput> {
    this.logger.debug('Layout graph ...');
    // Convert the graph to our internal structure (this is neat dispatching :-)
    const res = await converter(
      (id: NodeId, pos: Point) => new Body(id, pos),
      (b1: Body, b2: Body) => new Link(b1, b2)
    );
    this.bodies = res.bodies;
    this.links = res.links;

    // Setup force laws.
    this.forceLaws = await this.setupForceLaws();

    // dummy output, add one!
    const output: EngineOutput = [];
    this.bodies.forEach(body => output.push({ id: body.id, pos: body.position.add(Point.one.copy().scale(300)) }));

    return output;
  }

  async setupForceLaws(): Promise<ForceLaw[]> {
    return [];
  }
}
