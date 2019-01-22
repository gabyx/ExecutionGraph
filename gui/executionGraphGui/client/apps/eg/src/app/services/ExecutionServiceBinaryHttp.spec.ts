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

import { TestBed, async, getTestBed } from '@angular/core/testing';
import { HttpClientTestingModule, HttpTestingController } from '@angular/common/http/testing';

import { ExecutionServiceBinaryHttp } from './ExecutionServiceBinaryHttp';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';
import { LoggerFactory, SimpleConsoleLoggerFactory } from '@eg/logger';
import { BINARY_HTTP_ROUTER_BASE_URL } from '../tokens';

const TEST_URL = 'test';

describe('ExecutionServiceBinaryHttp', () => {
  let injector: TestBed;
  let service: ExecutionServiceBinaryHttp;
  let binaryHttpService: BinaryHttpRouterService;
  let httpMock: HttpTestingController;

  // Create a new service and mock before each test case
  beforeEach(() => {
    TestBed.configureTestingModule({
      imports: [HttpClientTestingModule],
      providers: [
        { provide: BINARY_HTTP_ROUTER_BASE_URL, useValue: TEST_URL},
        ExecutionServiceBinaryHttp,
        BinaryHttpRouterService,
        { provide: LoggerFactory, useClass: SimpleConsoleLoggerFactory }
      ]
    });
    injector = getTestBed();

    binaryHttpService = injector.get(BinaryHttpRouterService);
    service = injector.get(ExecutionServiceBinaryHttp);
    httpMock = injector.get(HttpTestingController);
  });
  // Verify that there are no outstanding requests
  afterEach(() => {
    httpMock.verify();
  });

  it('should return an execution result if successful', async(async () => {
    //@todo setup dummy response data
    const responseData = new ArrayBuffer(1);

    service.execute().then(() => {
      expect().nothing();
    });

    const req = httpMock.expectOne(`${TEST_URL}/graph/execute`);
    expect(req.request.method).toBe('POST');
    expect(req.request.headers.get('Content-Type')).toBe(binaryHttpService.binaryMimeType);

    req.flush(responseData);
  }));
});
