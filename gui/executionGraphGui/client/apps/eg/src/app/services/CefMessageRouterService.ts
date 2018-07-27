import { Injectable } from '@angular/core';
import * as msgpack from 'msgpack-lite';

type CefRequest = {
  request: string;
  persistent: boolean;
  onSuccess: (response: any) => any;
  onFailure: (errorCode: number, errorMessage: string) => void;
};

type CefWindow = {
  cefQuery: (request: CefRequest) => void;
};

/**
 * Router which sends JSON Payload (string) to the Backend by using
 * the registered `cefQuery` callback of the window instance.
 */
@Injectable()
export class CefMessageRouterService {
  private readonly cef: CefWindow;

  constructor() {
    if (!window.hasOwnProperty('cefQuery')) {
      console.error(`[CefMessageRouterService] No CEF available`);
    }
    this.cef = window as any;
  }

  public execute<T>(requestURL: string, payload: any): Promise<T> {
    console.log(`[CefMessageRouterService] Executing '${requestURL}'`);

    return new Promise((resolve, reject) => {
      var requestString = JSON.stringify({ requestURL: requestURL, payload: payload });

      this.cef.cefQuery({
        request: requestString,
        persistent: false,
        onSuccess: response => {
          console.log(`[CefMessageRouterService] Response for '${requestURL}': ${response}`);
          resolve(response);
        },
        onFailure: (errorCode, errorMessage) => {
          console.error(`[CefMessageRouterService] Request '${requestURL}' failed (${errorCode}): ${errorMessage}`);
          reject(errorMessage);
        }
      });
    });
  }
}
