
/** The XMLHttpRequest Options */
export interface RequestOptions {
    ignoreCache?: boolean;
    headers?: { [key: string]: string };
    timeout?: number; // 0 (or negative) to wait forever
}

/** The XMLHttpRequest Default-Options */
export const DefaultRequestOptions = {
    ignoreCache: false,
    headers: undefined,
    timeout: 5000, // default max duration for a request
};

export interface RequestResult {
    ok: boolean;
    status: number;
    statusText: string;
    data: Uint8Array;
    headers: string;
}

function queryParams(params: any = {}) {
    return Object.keys(params)
        .map(k => encodeURIComponent(k) + '=' + encodeURIComponent(params[k]))
        .join('&');
}

function withQuery(url: string, params: any = {}) {
    const queryString = queryParams(params);
    return queryString ? url + (url.indexOf('?') === -1 ? '?' : '&') + queryString : url;
}

function sucessResponse(xhr: XMLHttpRequest): RequestResult {
    return {
        ok: xhr.status >= 200 && xhr.status < 300,
        status: xhr.status,
        statusText: xhr.statusText,
        headers: xhr.getAllResponseHeaders(),
        data: xhr.response
    };
}

function errorResponse(xhr: XMLHttpRequest): RequestResult {
    return {
        ok: false,
        status: xhr.status,
        statusText: xhr.statusText,
        headers: xhr.getAllResponseHeaders(),
        data: null
    };
}

export function binaryXHRRequest(method: 'get' | 'post',
    url: string,
    queryParams: any = {},
    data: Uint8Array = null,
    options: RequestOptions = DefaultRequestOptions) : Promise<RequestResult> {

    return new Promise<RequestResult>((resolve, reject) => {

        const xhr = new XMLHttpRequest();
        xhr.open(method, withQuery(url, queryParams));

        // we expect a binary response
        xhr.responseType = "arraybuffer"

        const ignoreCache = options.ignoreCache
        const headers = options.headers
        const timeout = options.timeout

        if (headers) {
            Object.keys(headers).forEach(key => xhr.setRequestHeader(key, headers[key]));
        }

        if (ignoreCache) {
            xhr.setRequestHeader('Cache-Control', 'no-cache');
        }

        xhr.timeout = timeout;

        xhr.onload = evt => {
            resolve(sucessResponse(xhr));
        };

        xhr.onerror = evt => {
            console.warn(`Failed to make request: '${url}'`);
            reject(errorResponse(xhr));
        };

        xhr.ontimeout = evt => {
            console.warn(`Request took longer than expected: '${url}'`)
            reject(errorResponse(xhr, ));
        };

        if (method === 'post' && data) {
            xhr.send(data);
        } else {
            xhr.send();
        }
    });
}