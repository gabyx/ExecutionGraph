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
import { HttpClient, HttpHeaders, HttpErrorResponse } from '@angular/common/http'
import { _throw } from 'rxjs/observable/throw';
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/first';

import { flatbuffers } from 'flatbuffers';
import { GraphInfoService, Messages, sz } from './GraphInfoService';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';

@Injectable()
export class GraphInfoServiceBinaryHttp extends GraphInfoService {

  constructor(private readonly binaryRouter: BinaryHttpRouterService) {
    super();
  }

  public async getAllGraphTypeDescriptions(): Promise<Messages.GetAllGraphTypeDescriptionsResponse> {
    const result = await this.binaryRouter.get('general/getAllGraphTypeDescriptions');
    let buf = new flatbuffers.ByteBuffer(result);
    let response = Messages.GetAllGraphTypeDescriptionsResponse.getRootAsGetAllGraphTypeDescriptionsResponse(buf);
    console.log(`[GraphInfoServiceBinaryHttp] Received: Number of Graph types: ${response.graphsTypesLength()}`)

    let graphDesc = response.graphsTypes(0);
    console.log(`Sockets for graph 0:`);
    for(let i=0; i<graphDesc.socketTypeDescriptionsLength(); ++i)
    {
      let socketDesc = graphDesc.socketTypeDescriptions(i);
      console.log(`Socket: ${socketDesc.name()} [${socketDesc.type()}]`);
    }
    return response;
  }
}
