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
import { Point, Position, Vector2 } from '../../model/Point';
import { ILayoutEngine, GraphConverter, EngineOutput } from './ILayoutEngine';
import { LoggerFactory, ILogger } from '@eg/logger/src';
import { Injectable } from '@angular/core';
import { LayoutStrategys, ILayoutStrategy } from './ILayoutStrategy';
import { NodeId } from 'apps/eg/src/app/model';
import { Observable, generate, throwError, asyncScheduler, of } from 'rxjs';
import { map, switchMap, catchError, tap, finalize } from 'rxjs/operators';

type Force = Vector2;

abstract class ForceLaw {
  protected force: Force = Point.zero.copy();
  protected temp: Force = Point.zero.copy();

  public abstract accumulateForce(b1: Body, b2: Body): void;

  protected addToBody(b1: Body, b2: Body) {
    b1.extForce.add(this.force);
    b2.extForce.add(this.force);
  }
}

class SpringDamper extends ForceLaw {
  constructor(
    public readonly springConst: Vector2,
    public readonly damperConst: number,
    public readonly relaxedLength: Vector2
  ) {
    super();
  }
  public accumulateForce(b1: Body, b2: Body) {
    this.force.reset();
    // 2 Spring Forces: in X and Y seperate (for simplicity)
    Point.difference(b2.position, b1.position, this.temp); // signed gapDistance
    this.temp.subtract(this.relaxedLength); // gapDistance - relaxedLength
    this.temp.multiply(this.springConst); // FSpring = [cx 0; 0 cy] * (gapDistance - relaxedLength)
    this.force.add(this.temp);

    // 2 Damper Forces: in X and Y seperate (for simplicity)
    Point.difference(b2.velocity, b1.velocity, this.temp); // relative velocity
    this.temp.scale(this.damperConst); // FDamper = damperConst * (relative velocity)
    this.force.add(this.temp);

    this.addToBody(b1, b2);
  }
}

/**
 * This is a relaxed unilateral primitve function.
 * Page: 52. Figure 7.2
 * https://www.research-collection.ethz.ch/handle/20.500.11850/117165
 * Relaxed means we dont treat it as set-valued function
 * instead we just kind of make a normal function out of it :)
 *
 *    NormalForce
 *    ^ o
 *    | o
 *    | o
 *    | o
 *    |  o
 *    |   o
 *    |    o
 *    |      o
 *    |        o
 *    |          ooooooooooooooooo
 *    +--------------------------> GapDistance
 *
 * @class PenetrationForce
 * @extends {ForceLaw}
 */
class PenetrationForce extends ForceLaw {
  private gapDistance: number = 0;
  constructor(public readonly exponent: number) {
    super();
  }
  public accumulateForce(b1: Body, b2: Body) {
    this.force.reset();

    this.gapDistance = Point.difference(b2.position, b1.position, this.temp).length(); // gapDistance
    this.temp.normalize().scale(1 / Math.log(Math.pow(this.gapDistance + 1, this.exponent)));
    this.force.add(this.temp);

    this.addToBody(b1, b2);
  }
}

class Body {
  constructor(public readonly id: NodeId, public position: Position, public readonly opaqueData: any) {}
  public mass: number = 1.0;
  public velocity = Point.zero.copy();
  public oldPosition = Point.zero.copy();
  public geometry: string;

  public extForce: Force; // External force from all ForceLaws
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

  public readonly massRange: [number, number] = [1, 10];

  public readonly endTime: number = 2;

  public readonly maxSteps: number = 500;

  public readonly convergeRelTol: number = 1e-3;
  public readonly convergeAbsTol: number = 1e-3;

  public readonly springConst: Vector2 = Point.one.copy().scale(20);
  public readonly damperConst: Vector2 = Point.one.copy().scale(2);

  public readonly penetractionForceExponent = 40;

  public createEngine(loggerFactory: LoggerFactory): ILayoutEngine {
    return new MassSpringLayoutEngine(this, loggerFactory);
  }

  public createLinkForce() {
    return new SpringDamper(Point.one.copy().scale(2), 2, Point.one.copy().scale(300));
  }

  public createPenetrationForce() {
    return new PenetrationForce(this.penetractionForceExponent);
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
    return of(undefined).pipe(
      switchMap(() => this.setup(converter)),
      switchMap(() => this.setupForceLaws()),
      switchMap(() => this.runAsync())
    );
  }

  private runAsync(): Observable<EngineOutput> {
    this.logger.debug(`Run timestepping: ...`);
    const output: EngineOutput = new Array();
    this.bodies.forEach(body => output.push({ pos: body.position, id: body.id, opaqueData: body.opaqueData }));

    const deltaT = this.config.endTime / this.config.maxSteps;

    type State = { step: number; converged: boolean; time: number };
    const currState = { step: 0, converged: false, time: 0 };

    const logInterval = this.config.maxSteps / 10;

    // we use the same output instance for every emitted value
    // that should not be a problem!
    // if this state changes during the visualization (subscription)
    // we have incoherent times among the bodies which is acceptable :-)
    return generate(
      currState,
      (s: State) => {
        return s.time < this.config.endTime && !s.converged;
      },
      (s: State) => {
        ++s.step;

        if (s.step % logInterval == 0) {
          this.logger.debug(`Computed ${(s.step / this.config.maxSteps) * 100} %`);
        }
        s.time += deltaT;
        return s;
      },
      asyncScheduler
    ).pipe(
      tap(s => {
        // move the bodies
        this.bodies.forEach(body => {
          body.position.add(Point.one.copy().scale(1));
        });
      }),
      tap(s => {
        // update state
        let i = 0;
        this.bodies.forEach((body, id) => {
          output[i].pos = body.position;
          output[i++].id = body.id;
        });
      }),
      map(() => output),
      finalize(() => {
        this.logger.debug(`Run timestepping: [done]`);
      }),
      catchError(err => {
        this.logger.error(`${err}`);
        return throwError(err);
      })
    );
  }

  private async setup(converter: GraphConverter): Promise<void> {
    this.logger.debug('Setup graph: ...');
    // Convert the graph to our internal structure (this is neat dispatching :-)
    const res = await converter(
      (id: NodeId, pos: Point, opaqueData: any) => new Body(id, pos, opaqueData),
      (b1: Body, b2: Body) => new Link(b1, b2)
    );
    this.bodies = res.bodies;
    this.links = res.links;

    this.logger.debug('Setup graph: [done].');
  }

  async setupForceLaws(): Promise<ForceLaw[]> {
    this.logger.debug('Setup force laws: ...');

    // Setup for each link a link force.
    const linkForce = this.config.createLinkForce();
    this.links.forEach(l => {
      l.b1.forceLaws.push(linkForce);
      l.b2.forceLaws.push(linkForce);
    });

    // This hast stupidly quadtratic run-time performance and does not scale well!
    // us a GridAccelerator. ...(or KdTree -> :headbang: -> C++)
    const penetrationForce = this.config.createPenetrationForce();

    this.logger.debug('Setup force laws: [done]');
    return [];
  }
}
