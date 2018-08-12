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

import { Injectable, Inject, Testability } from '@angular/core';
import { HttpClient, HttpHeaders, HttpErrorResponse } from '@angular/common/http'
import { _throw } from 'rxjs/observable/throw';
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/first';

import { flatbuffers } from 'flatbuffers';
import { GeneralInfoService, sz } from './GeneralInfoService';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';

export class GeneralInfoServiceBinaryHttp extends GeneralInfoService {

  constructor(
    @Inject(BinaryHttpRouterService) private readonly binaryRouter: BinaryHttpRouterService,
    private readonly verboseLog?: boolean,
  ) {
    super();
  }

  public async getAllGraphTypeDescriptions(): Promise<sz.GetAllGraphTypeDescriptionsResponse> {
    const result = await this.binaryRouter.get('general/getAllGraphTypeDescriptions');
    let buf = new flatbuffers.ByteBuffer(result);
    let response = sz.GetAllGraphTypeDescriptionsResponse.getRootAsGetAllGraphTypeDescriptionsResponse(buf);
    console.log(`[GeneralInfoServiceBinaryHttp] Received: Number of Graph types: ${response.graphsTypesLength()}`)

    // Verbose logging the response if enabled
    if (this.verboseLog) {
      for (let g = 0; g < response.graphsTypesLength(); ++g) {
        let graphDesc = response.graphsTypes(g);
        console.log(`Infos for graph '${graphDesc.name()}' with id '${graphDesc.id()}' :`);
        console.log("Sockets:");
        for (let i = 0; i < graphDesc.socketTypeDescriptionsLength(); ++i) {
          let socketDesc = graphDesc.socketTypeDescriptions(i);
          console.log(`Socket: ${socketDesc.name()} [${socketDesc.type()}]`);
        }
        console.log("Nodes:");
        for (let i = 0; i < graphDesc.nodeTypeDescriptionsLength(); ++i) {
          let nodeDesc = graphDesc.nodeTypeDescriptions(i);
          console.log(`Node: ${nodeDesc.name()} [${nodeDesc.type()}]`);
        }
      }
    }

    return response;
  }
}
