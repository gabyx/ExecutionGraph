import { Injectable, Testability } from '@angular/core';
import { ExecutionService } from './ExecutionService';
import { CefMessageRouterService } from './CefMessageRouterService';
import { CefBinaryRouterService } from './CefBinaryRouterService';
import { HttpResponse } from '@angular/common/http' 
import { resolve } from 'url';
import 'flatbuffers';

@Injectable()
export class CefExecutionService extends ExecutionService {
  constructor(private readonly messageRouter: CefBinaryRouterService) {
    super();
  }

  public async execute(): Promise<void> {

    let requestUrl = 'general/DummyRequest';
    try {
      const result = await this.messageRouter.sendRequest(requestUrl, this.createBinaryData(null));

      let buf = new flatbuffers.ByteBuffer(result);
      console.debug(`[CefExecutionService] : response: size: ${result.byteLength}`);
    } catch(error) {
      console.error(`[CefExecutionService] : Request '${requestUrl}' errored with '${error}'!`);
    }
  }

  private createBinaryData(data: any): Uint8Array {
    return new Uint8Array([0x81, 0xa3, 0x66, 0x6f, 0x6f, 0xa3, 0x62, 0x61, 0x72]);
  }

}
