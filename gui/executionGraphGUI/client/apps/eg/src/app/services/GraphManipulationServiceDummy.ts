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

import { Injectable, Testability } from '@angular/core';
import { GraphManipulationService, sz} from './GraphManipulationService';
import { flatbuffers } from 'flatbuffers';

@Injectable()
export class GraphManipulationServiceDummy extends GraphManipulationService {

  constructor() {
    super();
  }

  public async addNode(graphId: string, type: string, name: string): Promise<sz.AddNodeResponse> {
    let builder = new flatbuffers.Builder(1024);

    console.debug(`[GeneralInfoServiceBinaryHttp] Not implemented!`)

    let buf = new flatbuffers.ByteBuffer(builder.asUint8Array());
    let response = sz.AddNodeResponse.getRootAsAddNodeResponse(buf);
    return response;
  }


}
