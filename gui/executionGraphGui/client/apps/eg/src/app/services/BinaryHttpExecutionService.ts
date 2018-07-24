import { Injectable, Testability } from '@angular/core';
import { HttpClient, HttpHeaders, HttpErrorResponse } from '@angular/common/http' 
import { _throw } from 'rxjs/observable/throw';
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/first';
import { flatbuffers } from 'flatbuffers';

import { ExecutionService } from './ExecutionService';

@Injectable()
export class BinaryHttpExecutionService extends ExecutionService {

  public baseUrl: string = `http://executiongraph-backend`;

  public binaryMimeType: string = 'application/octet-stream';

  constructor(private readonly httpClient: HttpClient)
  {
    super();
  }

  public async execute(): Promise<void> {
    const result = await this.postBinary('execution', this.createBinaryData(null));
    let buf = new flatbuffers.ByteBuffer(result);
  }

  private createBinaryData(data: any): Uint8Array {
    return new Uint8Array([0x81, 0xa3, 0x66, 0x6f, 0x6f, 0xa3, 0x62, 0x61, 0x72]);
  }

  private postBinary(path: string, payload: Uint8Array): Promise<Uint8Array> {
    const data = payload.buffer as ArrayBuffer;
    const url = `${this.baseUrl}/${path}`

    console.info(`[BinaryHttpExecutionService] Sending data to '${url}': (Bytes: '${data.byteLength}',  MIME-Type: '${this.binaryMimeType}')`);

    // Create a post request that returns an observable, which is kind of a stream of response events
    return this.httpClient.post(url, data, { responseType: 'arraybuffer', headers: new HttpHeaders({ 'Content-Type': this.binaryMimeType })})
      // Add a callback function that is executed whenever there is a new event in the request stream
      .do(responseData => console.log(`[BinaryHttpExecutionService] Received response (Bytes: ${responseData.byteLength})`))
      // In case of an error in the http event stream, catch it to log it and rethrow it, note that an error will only be actually thrown once
      // the observable is subscribed to, or in this case is converted into a promise and the promise is actually awaited.
      .catch((error: HttpErrorResponse) => {
        console.error(`[BinaryHttpExecutionService]: ${error.statusText}`);
        return _throw(error);
      })
      // After the first successful event in the stream, unsubscribe from the event stream
      .first()
      .map(responseData => new Uint8Array(responseData))
      // Convert the Observable to a Promise, so we're back to the "old-school" async await world instead of the "new-school" stream based approach
      .toPromise();
  }

}
