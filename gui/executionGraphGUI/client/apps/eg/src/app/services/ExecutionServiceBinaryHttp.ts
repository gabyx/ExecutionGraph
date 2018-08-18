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
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/first';
import { flatbuffers } from 'flatbuffers';

import { ExecutionService } from './ExecutionService';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';

@Injectable()
export class ExecutionServiceBinaryHttp extends ExecutionService {
  constructor(private readonly binaryRouter: BinaryHttpRouterService) {
    super();
  }

  public async execute(): Promise<void> {
    const result = await this.binaryRouter.post('graph/execute', this.createBinaryData(null));
    let buf = new flatbuffers.ByteBuffer(result);
  }

  private createBinaryData(data: any): Uint8Array {
    return new Uint8Array([0x81, 0xa3, 0x66, 0x6f, 0x6f, 0xa3, 0x62, 0x61, 0x72]);
  }
}
