import { Injectable, Testability } from '@angular/core';
import { HttpClient, HttpHeaders, HttpErrorResponse } from '@angular/common/http'
import { _throw } from 'rxjs/observable/throw';
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/first';

import { GraphInfoService, Messages, sz } from './GraphInfoService';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';

@Injectable()
export class GraphInfoServiceBinaryHttp extends GraphInfoService {

  constructor(private readonly binaryRouter: BinaryHttpRouterService) {
    super();
  }

  public async getAllGraphTypeDescriptions(): Promise<Messages.GetAllGraphTypeDescriptionsResponse> {
    const result = await this.binaryRouter.postBinary('getAllGraphTypeDescriptions', null);
    let buf = new flatbuffers.ByteBuffer(result);
    let response = Messages.GetAllGraphTypeDescriptionsResponse.getRootAsGetAllGraphTypeDescriptionsResponse(buf);
    console.debug(`[GraphInfoServiceBinaryHttp] Received: Number of Graph types: ${response.graphsTypesLength}`)
    return response;
  }
}
