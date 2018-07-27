import { TestBed, async, getTestBed } from '@angular/core/testing';
import { HttpClientTestingModule, HttpTestingController } from '@angular/common/http/testing';

import { ExecutionServiceBinaryHttp } from './ExecutionServiceBinaryHttp';

describe('ExecutionServiceBinaryHttp', () => {
    let injector: TestBed;
    let service: ExecutionServiceBinaryHttp;
    let httpMock: HttpTestingController;

    // Create a new service and mock before each test case
    beforeEach(() => {
        TestBed.configureTestingModule({
            imports: [HttpClientTestingModule],
            providers: [ExecutionServiceBinaryHttp]
        });
        injector = getTestBed();
        service = injector.get(ExecutionServiceBinaryHttp);
        httpMock = injector.get(HttpTestingController);
    });
    // Verify that there are no outstanding requests
    afterEach(() => {
        httpMock.verify();
    });

    it('should return an execution result if successful',
        async(async () => {
            //@todo setup dummy response data
            const responseData = new ArrayBuffer(1);

            service.execute().then(() => {
                expect().nothing();
            });

            const req = httpMock.expectOne(`${service.baseUrl}/execution`);
            expect(req.request.method).toBe("POST");
            expect(req.request.headers.get('Content-Type')).toBe(service.binaryMimeType);

            req.flush(responseData);


        })
    );
});
