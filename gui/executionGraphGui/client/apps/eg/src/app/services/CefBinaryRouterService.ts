import { Injectable } from '@angular/core';
import { HttpClient, HttpErrorResponse, HttpResponse, HttpHeaders } from '@angular/common/http';
import { Observable } from 'rxjs/Observable';
import { _throw } from 'rxjs/observable/throw';
import 'rxjs/add/operator/do';
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/first';
import * as ab2s from 'arraybuffer-to-string';

@Injectable()
export class CefBinaryRouterService {
    constructor(private httpClient: HttpClient) {}

    public async execute<T>(requestId: string, payload: any): Promise<void> {
        console.log('[CefBinaryRouterService] send()');
        await this.testSend(
            `http://executiongraph-backend/${requestId}`,
            this.createBinaryData(payload),
            'application/octet-stream'
        );
    }

    private async testSend(url: string, payload: Uint8Array, mimeType: string): Promise<void> {
        const data = payload.buffer as ArrayBuffer;

        console.info(
            `[CefBinaryRouterService]: sending data: bytes: '${
                data.byteLength
            }' with MIME type '${mimeType}' to '${url}'!`
        );
        // Create a post request that returns an observable, which is kind of a stream of response events
        let httpRequest: Observable<ArrayBuffer> = this.httpClient.post(url, data, {
            responseType: 'arraybuffer',
            headers: new HttpHeaders({ 'Content-Type': mimeType })
        });
        // Add a callback function that is executed whenever there is a new event in the request stream
        httpRequest = httpRequest.do(() => console.log(`[CefBinaryRouterService] success`));
        // In case of an error in the http event stream, catch it to log it and rethrow it, note that an error will only be actually thrown once
        // the observable is subscribed to, or in this case is converted into a promise and the promise is actually awaited.
        httpRequest = httpRequest.catch((error: HttpErrorResponse) => {
            console.error(`[CefBinaryRouterService]: ${error.statusText}`);
            return _throw(error);
        });
        // After the first successful event in the stream, unsubscribe from the event stream
        httpRequest = httpRequest.first();
        // Convert the Observable to a Promise, so we're back to the "old-school" async await world instead of the "new-school" stream based approach
        const requestPromise = httpRequest.toPromise();
        // Now await the promise to get the actual result data. This can only be done because the method is annotated with the async keyword.
        // The async keyword makes the method automatically return a promise Object. We could also return the promise straight away instead of awaiting it here
        // Which would actually be more efficient anyway. However if we would want to deserialize the reponse data we need access to the response data
        // here, which is why we would need to await the promise in order to get to the resopnse data...
        // Note: The await keyword leaves the current function, so the Event-Loop can continue, until the awaiting (Promise) is resolved, and
        // the function continues after the `await` keyword.
        const responseData: ArrayBuffer = await requestPromise;
        console.debug(
            `[CefBinaryRouterService] : response: size: ${responseData.byteLength} : '${ab2s(responseData, 'utf-8')}'`
        );
    }

    private createBinaryData(data: any): Uint8Array {
        return new Uint8Array([0x81, 0xa3, 0x66, 0x6f, 0x6f, 0xa3, 0x62, 0x61, 0x72]);
    }
}
