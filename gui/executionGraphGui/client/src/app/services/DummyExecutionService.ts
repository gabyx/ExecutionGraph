import { Injectable } from '@angular/core';
import { ExecutionService } from './ExecutionService';

@Injectable()
export class DummyExecutionService extends ExecutionService {

    constructor() {
        super();
    }

    public execute(): Promise<void> {
        console.log(`[DummyExecutionService] execute()`);
        return Promise.resolve();
    }
}