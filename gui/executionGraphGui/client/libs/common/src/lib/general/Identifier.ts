// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Tue Aug 14 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { Guid } from 'guid-typescript';

export class Id {
  private readonly _id: Guid;
  private readonly _idString: string;

  constructor(id?: string) {
    if (id) {
      this._id = Guid.parse(id);
    } else {
      this._id = Guid.create();
    }
    this._idString = this._id.toString();
  }

  public get guid(): Guid {
    return this._id;
  }

  public id(): string {
    return this._idString;
  }

  public equals(id: Id): boolean {
    return id != null && this.guid === id.guid;
  }
}

export class IdNamed extends Id {
  constructor(private readonly _name: string, id?: string) {
    super(id);
  }

  public get name(): string {
    return `${this._name}-${super.id()}`;
  }
  public get shortName(): string {
    return this._name;
  }
}
