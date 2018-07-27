import { Injectable, Testability } from '@angular/core';
import { HttpClient, HttpHeaders, HttpErrorResponse } from '@angular/common/http'
import { _throw } from 'rxjs/observable/throw';
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/first';
import { flatbuffers } from 'flatbuffers';

import { ExecutionService } from './ExecutionService';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';

@Injectable()
export class ExecutionServiceBinaryHttp extends ExecutionService {

  constructor(private readonly binaryRouter: BinaryHttpRouterService)
  {
    super();
  }

  public async execute(): Promise<void> {
    const result = await this.binaryRouter.postBinary('execute', this.createBinaryData(null));
    let buf = new flatbuffers.ByteBuffer(result);
  }

  private createBinaryData(data: any): Uint8Array {
    return new Uint8Array([0x81, 0xa3, 0x66, 0x6f, 0x6f, 0xa3, 0x62, 0x61, 0x72]);
  }



}
