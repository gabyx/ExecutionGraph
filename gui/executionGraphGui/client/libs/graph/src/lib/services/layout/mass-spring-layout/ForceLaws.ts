// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Mon Jan 21 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { Body, Force } from './Body';
import { Vector2 } from '@eg/common';

export abstract class ForceLawInteraction {
  protected force: Force = Vector2.zero.copy();
  protected temp: Force = Vector2.zero.copy();

  public abstract accumulateForce(b1: Body, b2: Body): void;

  protected addToBody(b1: Body, b2: Body) {
    b1.extForce.add(this.force);
    b2.extForce.add(this.force.negate());
  }
}

export abstract class ForceLawExternal {
  protected force: Force = Vector2.zero.copy();

  public abstract accumulateForce(b1: Body): void;

  protected addToBody(b1: Body) {
    b1.extForce.add(this.force);
  }
}

export class SpringDamperInteraction extends ForceLawInteraction {
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
export class PenetrationForceInteraction extends ForceLawInteraction {
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
