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
import { ILayoutEngine, GraphConverter, EngineOutput } from '../ILayoutEngine';
import { LoggerFactory, ILogger } from '@eg/logger';
import { Injectable } from '@angular/core';
import { Observable, generate, throwError, asyncScheduler, of } from 'rxjs';
import { map, switchMap, catchError, finalize, takeWhile } from 'rxjs/operators';
import { Vector2 } from '@eg/common';
import { ForceLawInteraction, ForceLawExternal } from './ForceLaws';
import { Body } from './Body';
import { Link } from './Link';
import { MassSpringLayoutConfig } from './MassSpringLayoutConfig';

interface ForceInteraction {
  b1: Body;
  b2: Body;
  forceLaw: ForceLawInteraction;
}
interface ExtForce {
  b: Body;
  forceLaw: ForceLawExternal;
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

  constructor(private readonly config: MassSpringLayoutConfig, loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('MassSpringLayoutEngine');
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
    this.bodies.forEach(body => output.push({ pos: body.position, opaqueData: body.opaqueData }));

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
    this.bodies = [];
    this.links = [];

    await converter(
      (s: { pos: Vector2; opaqueData: any }) => {
        const b = new Body(s.pos, s.opaqueData);
        this.bodies.push(b);
        return b;
      },
      (b1: Body, b2: Body) => {
        const l = new Link(b1, b2);
        this.links.push(l);
        return l;
      }
    );

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
