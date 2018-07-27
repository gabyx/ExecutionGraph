import { Injectable } from '@angular/core';
import { ExecutionService } from './ExecutionService';

@Injectable()
export class ExecutionServiceDummy extends ExecutionService {
  constructor() {
    super();
  }

  public execute(): Promise<void> {
    console.log(`[ExecutionServiceDummy] execute()`);
    return Promise.resolve();
  }
}
