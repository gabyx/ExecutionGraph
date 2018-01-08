import { Injectable } from '@angular/core';

type CefRequest = {
    request: string, 
    persistent: boolean,
    onSuccess: (response: any) => any,
    onFailure: (errorCode: number, errorMessage: string) => void
};

type CefWindow = {
    cefQuery: (request: CefRequest) => void
};

@Injectable()
export class CefMessageRouterService {

    private readonly cef: CefWindow;

    constructor() {
        if(!window.hasOwnProperty("cefQuery")) {
            console.error(`[CefMessageRouterService] No CEF available`);
        }
        this.cef = window as any;
    }

    public execute<T>(requestId: string, payload: any) : Promise<T> {
        console.log(`[CefMessageRouterService] Executing '${requestId}'`);

        return new Promise((resolve, reject) => {
            var requestString = JSON.stringify({requestId: requestId, payload: payload});
            this.cef.cefQuery({
                request: requestString,
                persistent: false,
                onSuccess: response => {
                    console.log(`[CefMessageRouterService] Response for '${requestId}': ${response}`);
                    resolve(response);
                },
                onFailure: (errorCode, errorMessage) => {
                    console.error(`[CefMessageRouterService] Request '${requestId}' failed (${errorCode}): ${errorMessage}`);
                    reject(errorMessage);
                }
            });
        });
    }
}