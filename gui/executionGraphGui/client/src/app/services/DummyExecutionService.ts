import { Injectable } from '@angular/core';
import { ExecutionService } from './ExecutionService';

import * as msgpack from 'msgpack-lite';
import * as ab2s from 'arraybuffer-to-string';

@Injectable()
export class DummyExecutionService extends ExecutionService {

    constructor() {
        super();
    }

    public execute(): Promise<void> {
        return new Promise((resolve, reject) => {

            var requestBinary = msgpack.encode({
                "requestId": "executeGraph",
                "payload": {
                    "graphId": 12,
                    "args" : [1,2,3]
                }
            }) as Uint8Array;
            console.log("Dummy binary:", requestBinary);
            console.log("Dummy binary as string:", ab2s(requestBinary, 'binary'));
            console.log(`[DummyExecutionService] execute()`);
            resolve();
        })
    }
}