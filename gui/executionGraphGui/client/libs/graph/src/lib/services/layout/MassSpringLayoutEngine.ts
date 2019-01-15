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
import { ILayoutEngine, GraphConverter, EngineOutput, EngineOutputState, createOutputState } from './ILayoutEngine';
import { LoggerFactory, ILogger } from '@eg/logger/src';
import { Injectable } from '@angular/core';
import { LayoutStrategys, ILayoutStrategy } from './ILayoutStrategy';
import { NodeId } from 'apps/eg/src/app/model';
import { from, Observable, Observer, generate } from 'rxjs';
import { ENGINE_METHOD_ALL } from 'constants';
import { map, switchMap } from 'rxjs/operators';
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
  constructor(public readonly id: NodeId, public position: Position) {}
  public mass: number = 1.0;
  public velocity = Point.zero.copy();
  public oldPosition = Point.zero.copy();
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
  endTime: number = 2;

  maxSteps: 100;

  convergeRelTol: number = 1e-3;
  convergeAbsTol: number = 1e-3;

  public createEngine(loggerFactory: LoggerFactory): ILayoutEngine {
    return new MassSpringLayoutEngine(this, loggerFactory);
  }
}

@Injectable()
export class MassSpringLayoutEngine extends ILayoutEngine {
  private readonly logger: ILogger;

  private bodies: BodyMap;
  private links: Link[];
  private forceLaws: ForceLaw[];

  constructor(private readonly config: MassSpringLayoutStrategy, private readonly loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('MassSpringLayoutEngine');
  }

  public isStrategyValid(s: LayoutStrategys) {
    return s === MassSpringLayoutStrategy.strategy_;
  }

  public run(converter: GraphConverter): Observable<EngineOutput> {
    return from(this.setup(converter)).pipe(switchMap(() => this.runAsync()));
  }

  private runAsync(): Observable<EngineOutput> {
    const output: EngineOutput = Array.from({ length: this.bodies.size }, () => createOutputState());
    const deltaT = this.config.endTime / this.config.maxSteps;

    type State = { step: number; converged: boolean; time: number };
    const currState = { step: 0, converged: false, time: 0 };

    // we use the same output instance for every emitted value
    // that should not be a problem!
    // if this state changes during the visualization (subscription)
    // we have incoherent times among the bodies which acceptable :-)
    return generate(
      currState,
      (s: State) => {
        return s.time < this.config.endTime && !s.converged;
      },
      (s: State) => {
        // move the bodies
        this.bodies.forEach(body => body.position.add(Point.zero.copy().scale(20)));

        // update state
        let i = 0;
        this.bodies.forEach((body, id) => {
          output[i].pos = body.position;
          output[i++].id = body.id;
        });

        this.logger.debug(output[0].pos);

        s.time += deltaT;
        return s;
      }
    ).pipe(map(() => output));
  }

  private async setup(converter: GraphConverter): Promise<void> {
    this.logger.debug('Layout graph: setup ...');
    // Convert the graph to our internal structure (this is neat dispatching :-)
    const res = await converter(
      (id: NodeId, pos: Point) => new Body(id, pos),
      (b1: Body, b2: Body) => new Link(b1, b2)
    );
    this.bodies = res.bodies;
    this.links = res.links;

    // Setup force laws.
    this.forceLaws = await this.setupForceLaws();
    this.logger.debug('Layout graph: setup finished ...');
  }

  async setupForceLaws(): Promise<ForceLaw[]> {
    return [];
  }
}
