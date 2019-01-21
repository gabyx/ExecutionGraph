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

interface CefRequest {
  request: string;
  persistent: boolean;
  onSuccess: (response: any) => any;
  onFailure: (errorCode: number, errorMessage: string) => void;
}
interface CefWindow {
  cefQuery: (request: CefRequest) => void;
}

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
      const requestString = JSON.stringify({
        requestURL: requestURL,
        payload: payload
      });

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
