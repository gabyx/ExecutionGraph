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
import { Observable, generate, throwError, asyncScheduler, of } from 'rxjs';
import { map, switchMap, catchError, tap, finalize, takeWhile } from 'rxjs/operators';
import { Vector2 } from '@eg/common/src';

type Force = Vector2;

abstract class ForceLawInteraction {
  protected force: Force = Vector2.zero.copy();
  protected temp: Force = Vector2.zero.copy();

  public abstract accumulateForce(b1: Body, b2: Body): void;

  protected addToBody(b1: Body, b2: Body) {
    b1.extForce.add(this.force);
    b2.extForce.add(this.force.negate());
  }
}

abstract class ForceLawExternal {
  protected force: Force = Vector2.zero.copy();

  public abstract accumulateForce(b1: Body): void;

  protected addToBody(b1: Body) {
    b1.extForce.add(this.force);
  }
}

class SpringDamperInteraction extends ForceLawInteraction {
  private sign = Vector2.one.copy();

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
    Vector2.difference(b2.position, b1.position, this.temp); // signed gapDistance
    this.sign.x = Math.sign(this.temp.x);
    this.sign.y = Math.sign(this.temp.y);

    this.temp
      .abs()
      .subtract(this.relaxedLength)
      .multiply(this.springConst)
      .multiply(this.sign);
    // FSpring_x = cx * gapDistance.x/|gapDistance.x| * (|gapDistance.x| - relaxedLength.x)

    this.force.add(this.temp);

    // 2 Damper Forces: in X and Y seperate (for simplicity)
    Vector2.difference(b2.velocity, b1.velocity, this.temp); // relative velocity
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
class PenetrationForceInteraction extends ForceLawInteraction {
  private gapDistance: number = 0;
  constructor(public readonly exponent: number) {
    super();
  }
  public accumulateForce(b1: Body, b2: Body) {
    this.force.reset();

    this.gapDistance = Vector2.difference(b2.position, b1.position, this.temp).length(); // gapDistance
    this.temp.normalize().scale(1 / Math.log(Math.pow(this.gapDistance + 1, this.exponent)));
    this.force.add(this.temp);

    this.addToBody(b1, b2);
  }
}

interface ForceInteraction {
  b1: Body;
  b2: Body;
  forceLaw: ForceLawInteraction;
}
interface ExtForce {
  b: Body;
  forceLaw: ForceLawExternal;
}

class Body {
  private temp: Position = Vector2.zero.copy();
  constructor(public readonly id: NodeId, position: Position, public readonly opaqueData: any) {
    this.position = position;
    this.positionEnd = position.copy();
  }
  public massInv: number = 1.0 / 1.0;

  public position: Position;
  public positionEnd: Position;

  public velocity = Vector2.zero.copy();
  public velocityEnd = Vector2.zero.copy();

  public extForce: Force = Vector2.zero.copy(); // External force from all ForceLaws

  public async integrateToMidPoint(deltaT: number): Promise<void> {
    Vector2.scale(this.velocity, 0.5 * deltaT, this.temp);
    this.position.add(this.temp);
  }

  public async computeEndVelocity(deltaT: number): Promise<void> {
    Vector2.scale(this.extForce, deltaT * this.massInv, this.velocityEnd).add(this.velocity);
  }

  public async integrateToEndPoint(deltaT: number): Promise<void> {
    Vector2.sum(this.velocityEnd, this.velocity, this.positionEnd)
      .scale(0.5 * deltaT)
      .add(this.position);
  }

  public resetForNextIteration() {
    [this.position, this.positionEnd] = [this.positionEnd, this.position]; // Swap Positions;
    [this.velocity, this.velocityEnd] = [this.velocityEnd, this.velocity]; // Swap Positions;
    this.extForce.reset();
  }

  public async hasPositionConverged(relTol: number, absTol: number): Promise<boolean> {
    Vector2.difference(this.positionEnd, this.position, this.temp).abs();
    // Check convergence in both direction instead of taking the norm... (to expensive...)
    return (
      this.temp.x <= relTol * Math.abs(this.position.x) + absTol &&
      this.temp.y <= relTol * Math.abs(this.position.y) + absTol
    );
  }
}

class Link {
  constructor(public readonly b1: Body, public readonly b2: Body) {}
}

type BodyMap = Map<NodeId, Body>;

export class MassSpringLayoutStrategy extends ILayoutStrategy {
  public static readonly strategy_ = LayoutStrategys.MassSpringSystem;
  public readonly strategy = MassSpringLayoutStrategy.strategy_;

  public readonly massRange: [number, number] = [1, 10];

  public readonly endTime: number = 10;
  public readonly maxSteps: number = 300;

  public readonly convergeRelTol: number = 1e-4;
  public readonly convergeAbsTol: number = 1e-4;

  public readonly springConst = new Vector2([10, 5]);
  public readonly damperConst = 2;
  public readonly relaxedLength = new Vector2([300, 50]);

  public readonly penetractionForceExponent = 40;

  public createEngine(loggerFactory: LoggerFactory): ILayoutEngine {
    return new MassSpringLayoutEngine(this, loggerFactory);
  }

  public createLinkForce() {
    return new SpringDamperInteraction(this.springConst, this.damperConst, this.relaxedLength);
  }

  public createContactForce() {
    return new PenetrationForceInteraction(this.penetractionForceExponent);
  }
}

interface State {
  step: number;
  converged: boolean;
  time: number;
}

@Injectable()
export class MassSpringLayoutEngine extends ILayoutEngine {
  private readonly logger: ILogger;

  private bodies: Body[];
  private links: Link[];

  private extForceInteraction: ForceInteraction[];
  private extForce: ExtForce[];

  constructor(private readonly config: MassSpringLayoutStrategy, private readonly loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('MassSpringLayoutEngine');
  }

  public isStrategyValid(s: LayoutStrategys) {
    return s === MassSpringLayoutStrategy.strategy_;
  }

  public run(converter: GraphConverter): Observable<EngineOutput> {
    return of(0).pipe(
      switchMap(() => {
        return this.setup(converter);
      }),
      switchMap(() => this.runAsync())
    );
  }

  private runAsync(): Observable<EngineOutput> {
    this.logger.debug(`Run timestepping: ...`);
    const output: EngineOutput = new Array();
    this.bodies.forEach(body => output.push({ pos: body.position, id: body.id, opaqueData: body.opaqueData }));

    const deltaT = this.config.endTime / this.config.maxSteps;
    const state = { step: 0, converged: false, time: 0 };
    const logInterval = this.config.maxSteps / 10;

    // we use the same output instance for every emitted value
    // that should not be a problem!
    // if this state changes during the visualization (subscription)
    // we have incoherent times among the bodies which is acceptable :-)
    return generate(
      state,
      () => true,
      (s: State) => {
        ++s.step;

        if (s.step % logInterval === 0) {
          this.logger.debug(`Computed ${(s.step / this.config.maxSteps) * 100} %`);
        }

        s.time += deltaT;
        return s;
      },
      asyncScheduler
    ).pipe(
      switchMap(s => this.doTimeStep(s, deltaT)),
      takeWhile(s => {
        return s.time < this.config.endTime && !s.converged;
      }),
      catchError(err => {
        this.logger.error(`${err}`);
        return throwError(err);
      }),
      finalize(() => {
        this.logger.debug(`Run timestepping: [done] [converged: ${state.converged}]`);
      }),
      map(() => output)
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

    await this.setupForceLaws();

    this.logger.debug('Setup graph: [done].');
  }

  async setupForceLaws(): Promise<void> {
    this.logger.debug('Setup force laws: ...');
    this.extForceInteraction = [];
    this.extForce = [];

    // Setup for each link a link force.
    const linkForceLaw = this.config.createLinkForce();
    this.links.forEach(l => {
      this.extForceInteraction.push({ b1: l.b1, b2: l.b2, forceLaw: linkForceLaw });
    });

    // // This hast stupidly quadtratic run-time performance and does not scale well!
    // // us a GridAccelerator. ...(or KdTree -> :headbang: -> C++)
    // const contactForce = this.config.createContactForce();
    // this.bodies.forEach(b1 => {
    //   this.bodies.forEach(b2 => {
    //     if (b2.id.greaterThan(b1.id)) {
    //       this.extForceInteraction.push({ b1: b1, b2: b2, forceLaw: contactForce });
    //     }
    //   });
    // });

    this.logger.debug('Setup force laws: [done]');
  }

  private async doTimeStep(s: State, deltaT: number): Promise<State> {
    await this.integrateToMidPoint(deltaT);
    await this.computeForces();
    await this.computeEndVelocity(deltaT);
    await this.integrateToEndPoint(deltaT);
    s.converged = await this.checkConvergence();
    await this.finalizeTimeStep();
    return s;
  }

  private async integrateToMidPoint(deltaT: number): Promise<void> {
    for (const b of this.bodies) {
      await b.integrateToMidPoint(deltaT);
    }
  }

  private async computeForces(): Promise<void> {
    for (const f of this.extForce) {
      await f.forceLaw.accumulateForce(f.b);
    }
    for (const f of this.extForceInteraction) {
      await f.forceLaw.accumulateForce(f.b1, f.b2);
    }
  }

  private async computeEndVelocity(deltaT: number): Promise<void> {
    for (const b of this.bodies) {
      await b.computeEndVelocity(deltaT);
    }
  }

  private async integrateToEndPoint(deltaT: number): Promise<void> {
    for (const b of this.bodies) {
      await b.integrateToEndPoint(deltaT);
    }
  }

  private async checkConvergence(): Promise<boolean> {
    let i = 0;
    for (const b of this.bodies) {
      const conv = await b.hasPositionConverged(this.config.convergeRelTol, this.config.convergeAbsTol);
      i += conv ? 1 : 0;
    }
    return i === this.bodies.length;
  }

  private async finalizeTimeStep(): Promise<void> {
    for (const b of this.bodies) {
      b.resetForNextIteration();
    }
  }
}
