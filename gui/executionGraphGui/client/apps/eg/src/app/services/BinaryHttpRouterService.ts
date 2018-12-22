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

import { Injectable, Inject } from '@angular/core';
import { HttpClient, HttpHeaders, HttpErrorResponse } from '@angular/common/http';
import { throwError } from 'rxjs';
import { tap, map, catchError, first } from 'rxjs/operators';
import { LoggerFactory, ILogger } from '@eg/logger';
import { BINARY_HTTP_ROUTER_BASE_URL } from '../tokens';

/**
 * Router which sends binary payload to the backend using HTTP requests.
 */
@Injectable()
export class BinaryHttpRouterService {
  public binaryMimeType: string = 'application/octet-stream';
  private logger: ILogger;

  constructor(
    private httpClient: HttpClient,
    private loggerFactory: LoggerFactory,
    @Inject(BINARY_HTTP_ROUTER_BASE_URL) private readonly baseUrl: string
  ) {
    this.logger = loggerFactory.create('BinaryHttpRouterService');
  }

  /**
   * Get the response payload as a Uint8Array.
   * @param requestUrl The request url.
   */
  public get(requestUrl: string): Promise<Uint8Array> {
    const url = `${this.baseUrl}/${requestUrl}`;

    this.logger.debug(`Get from '${url})`);

    // Create a get request that returns an observable, which is kind of a stream of response events
    return (
      this.httpClient
        .get(url, { responseType: 'arraybuffer' })
        .pipe(
          // Add a callback function that is executed whenever there is a new event in the request stream
          tap(responseData => this.logger.debug(`Received response (Bytes: ${responseData.byteLength})`)),
          // In case of an error in the http event stream, catch it to log it and rethrow it, note that an error will only be actually thrown once
          // the observable is subscribed to, or in this case is converted into a promise and the promise is actually awaited.
          catchError((error: HttpErrorResponse) => {
            this.logger.error(`${error.message} (status: ${error.status})`);
            return throwError(error);
          }),
          // After the first successful event in the stream, unsubscribe from the event stream
          first(),
          map(responseData => new Uint8Array(responseData))
        )
        // Convert the Observable to a Promise, so we're back to the "old-school" async await world instead of the "new-school" stream based approach
        .toPromise()
    );
  }

  /**
   * Post the binary request, and get a the response payload as a Uint8Array.
   * @param requestUrl The request url.
   * @param payload The request payload.
   */
  public post(requestUrl: string, payload: Uint8Array): Promise<Uint8Array> {
    // @todo: This .slice is horribly inefficient, how to convert a payload with byteOffset != 0
    // to a ArrayBuffer -> its stupidly impossible...
    const data = (payload.byteOffset > 0 ? payload.slice().buffer : payload.buffer) as ArrayBuffer;
    const url = `${this.baseUrl}/${requestUrl}`;

    this.logger.debug(`Post to '${url}': (Bytes: '${data.byteLength}')`);

    // Create a post request that returns an observable, which is kind of a stream of response events
    return (
      this.httpClient
        .post(url, data, {
          responseType: 'arraybuffer',
          headers: new HttpHeaders({ 'Content-Type': this.binaryMimeType })
        })
        .pipe(
          // Add a callback function that is executed whenever there is a new event in the request stream
          tap(responseData => this.logger.debug(`Received response (Bytes: ${responseData.byteLength})`)),
          // In case of an error in the http event stream, catch it to log it and rethrow it, note that an error will only be actually thrown once
          // the observable is subscribed to, or in this case is converted into a promise and the promise is actually awaited.
          catchError((error: HttpErrorResponse) => {
            this.logger.error(`${error.message} (status: ${error.status})`);
            return throwError(error);
          }),
          // After the first successful event in the stream, unsubscribe from the event stream
          first(),
          map(responseData => new Uint8Array(responseData))
        )
        // Convert the Observable to a Promise, so we're back to the "old-school" async await world instead of the "new-school" stream based approach
        .toPromise()
    );
  }
}
