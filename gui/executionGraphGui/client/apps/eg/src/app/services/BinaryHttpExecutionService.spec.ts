import { TestBed, async, getTestBed } from '@angular/core/testing';
import { HttpClientTestingModule, HttpTestingController } from '@angular/common/http/testing';

import { BinaryHttpExecutionService } from './BinaryHttpExecutionService';

describe('BinaryHttpExecutionService', () => {
    let injector: TestBed;
    let service: BinaryHttpExecutionService;
    let httpMock: HttpTestingController;
    
    // Create a new service and mock before each test case
    beforeEach(() => {
        TestBed.configureTestingModule({
            imports: [HttpClientTestingModule],
            providers: [BinaryHttpExecutionService]
        });
        injector = getTestBed();
        service = injector.get(BinaryHttpExecutionService);
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
