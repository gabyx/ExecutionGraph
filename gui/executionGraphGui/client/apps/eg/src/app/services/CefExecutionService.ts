import { Injectable } from '@angular/core';
import { ExecutionService } from './ExecutionService';
import { CefMessageRouterService } from './CefMessageRouterService';
import { CefBinaryRouterService } from './CefBinaryRouterService';
import { HttpResponse } from '@angular/common/http'

@Injectable()
export class CefExecutionService extends ExecutionService {
  constructor(private readonly messageRouter: CefBinaryRouterService) {
    super();
  }

  public execute(): Promise<void> {

    let requestUrl = 'general/DummyRequest';
    return this.messageRouter.sendRequest(requestUrl, this.createBinaryData(null))
      .then((response: HttpResponse<ArrayBuffer>) => {
        console.debug(`[CefExecutionService] : response: size: ${response.body.byteLength}`);
      })
      .catch((error: any) => {
        console.error(`[CefExecutionService] : Request '${requestUrl}' errored with '${error}'!`);
      });
    //return this.messageRouter.sendRequest('general/getAllGraphTypeDescriptions', null);
  }

  private createBinaryData(data: any): Uint8Array {
    return new Uint8Array([0x81, 0xa3, 0x66, 0x6f, 0x6f, 0xa3, 0x62, 0x61, 0x72]);
  }

}
