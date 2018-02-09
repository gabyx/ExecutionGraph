import { Injectable } from '@angular/core';
import { ExecutionService } from './ExecutionService';

import { binaryXHRRequest, RequestResult } from '../common/BinaryXHRRequest';
import * as msgpack from 'msgpack-lite';

@Injectable()
export class DummyExecutionService extends ExecutionService {

    constructor() {
        super();
    }

    public execute(): Promise<void> {
        console.log(`[DummyExecutionService] execute()`);

        var message = msgpack.encode({
            "requestId": "addNode",
            "payload": {
                "graphId": 12,
                "args": [1, 2, 3]
            }
        }) as Uint8Array;

        return new Promise((resolve, reject) => {
            binaryXHRRequest('post', "backend://executionGraph/addNode", undefined , message).then(
                (requestResult: RequestResult) => {
                    console.log(`[DummyExecutionService] binaryXHRRequest() success: ${requestResult.statusText}`);
                    resolve("`[DummyExecutionService] binaryXHRRequest() success");
                },
                (requestResult: RequestResult) => {
                    console.log(`[DummyExecutionService] binaryXHRRequest() failed: ${requestResult.statusText}`);
                    reject("[DummyExecutionService] binaryXHRRequest() failed");
                });
        })
    }
}