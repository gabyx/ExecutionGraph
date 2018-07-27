import { Injectable, Testability } from '@angular/core';
import { HttpClient, HttpHeaders, HttpErrorResponse } from '@angular/common/http'
import { _throw } from 'rxjs/observable/throw';
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/first';

/**
 * Router which sends binary payload to the backend using HTTP requests.
 */
@Injectable()
export class BinaryHttpRouterService {

  public baseUrl: string = `http://executiongraph-backend`;
  public binaryMimeType: string = 'application/octet-stream';

  constructor(private httpClient: HttpClient) { }

  // Post the binary request, and get a the response payload as a ArrayBuffer promise.
  public postBinary(requestUrl: string, payload: Uint8Array): Promise<Uint8Array> {
    const data = payload.buffer as ArrayBuffer;
    const url = `${this.baseUrl}/${requestUrl}`

    console.info(`[BinaryHttpRouterService] Sending data to '${url}': (Bytes: '${data.byteLength}',  MIME-Type: '${this.binaryMimeType}')`);

    // Create a post request that returns an observable, which is kind of a stream of response events
    return this.httpClient.post(url, data, { responseType: 'arraybuffer', headers: new HttpHeaders({ 'Content-Type': this.binaryMimeType }) })
      // Add a callback function that is executed whenever there is a new event in the request stream
      .do(responseData => console.log(`[BinaryHttpRouterService] Received response (Bytes: ${responseData.byteLength})`))
      // In case of an error in the http event stream, catch it to log it and rethrow it, note that an error will only be actually thrown once
      // the observable is subscribed to, or in this case is converted into a promise and the promise is actually awaited.
      .catch((error: HttpErrorResponse) => {
        console.error(`[BinaryHttpRouterService]: ${error.statusText}`);
        return _throw(error);
      })
      // After the first successful event in the stream, unsubscribe from the event stream
      .first()
      .map(responseData => new Uint8Array(responseData))
      // Convert the Observable to a Promise, so we're back to the "old-school" async await world instead of the "new-school" stream based approach
      .toPromise();
  }
}
