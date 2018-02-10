import { Injectable } from '@angular/core';
import { HttpClient, HttpErrorResponse } from '@angular/common/http';
import { _throw } from 'rxjs/observable/throw';
import 'rxjs/add/operator/do';
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/first';
import * as msgpack from 'msgpack-lite';

import { ExecutionService } from './ExecutionService';

@Injectable()
export class BinaryHttpExecutionService extends ExecutionService {

    constructor(private httpClient: HttpClient) {
        super();
    }

    public async execute(): Promise<void> {
        console.log(`[BinaryHttpExecutionService] execute()`);

        const data = this.createBinaryData();
        const responseData = await this.httpClient.post("backend://executionGraph/addNode", data, { responseType: "arraybuffer" })
            .do(() => console.log(`[BinaryHttpExecutionService] success`))
            .catch((error: HttpErrorResponse) => {
                console.error(`[BinaryHttpExecutionService]: ${error.statusText}`);
                return _throw(error);
            })

            .first()
            .toPromise();
    }
        
    private createBinaryData(): Uint8Array {
        return msgpack.encode({
            "requestId": "addNode",
            "payload": {
                "graphId": 12,
                "args": [1, 2, 3]
            }
        }) as Uint8Array;
    }
}