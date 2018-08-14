// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jul 29 2018
//  @author Simon Spoerri, simon (døt) spoerri (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { Injectable } from '@angular/core';
import { Identifier } from "@eg/comon/Identifier"
import { GraphManagementService, sz } from './GraphManagementService';
import { flatbuffers } from 'flatbuffers';

@Injectable()
export class GraphManagementServiceDummy extends GraphManagementService {

  constructor() {
    super();
  }

  private readonly _id = new Identifier("GraphManipulationServiceDummy");
  public get id(): Identifier { return this._id; }

  public async addGraph(graphTypeId: string): Promise<string> {
    console.error(`[${this.id.name}] Not implemented yet!`);
    return "HERE 16byte GUID"
  }

  public async removeGraph(graphId: string): Promise<void> {
    console.error(`[${this.id.name}] Not implemented yet!`);
  }
}
