Specifications for Client and Backend Communication
===
In this document *client* is referred to the Angular implementation in `./client` which provides the GUI (HTML/CSS/TypeScript) implementation. The *backend*  is referred to the C++ implementation in `./backend`.

The communication between *client* and *backend* is done over the `./client/.../CefMessageRouterService.ts` which executes the pre-registered `window.cef.cefQuery` on the *client* side and the `./cefapp/MessageHandler

