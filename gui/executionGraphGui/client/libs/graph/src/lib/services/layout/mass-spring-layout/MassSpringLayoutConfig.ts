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
import { Vector2 } from '@eg/common';
import { ForceLawInteraction, SpringDamperInteraction, PenetrationForceInteraction } from './ForceLaws';

export class MassSpringLayoutConfig {
  public readonly massRange: [number, number] = [1, 10];

  public readonly endTime: number = 10;
  public readonly maxSteps: number = 300;

  public readonly convergeRelTol: number = 1e-4;
  public readonly convergeAbsTol: number = 1e-4;

  public readonly springConst = new Vector2([10, 5]);
  public readonly damperConst = 2;
  public readonly relaxedLength = new Vector2([300, 50]);

  public readonly penetractionForceExponent = 40;

  public createLinkForce(): ForceLawInteraction {
    return new SpringDamperInteraction(this.springConst, this.damperConst, this.relaxedLength);
  }

  public createContactForce(): ForceLawInteraction {
    return new PenetrationForceInteraction(this.penetractionForceExponent);
  }
}
