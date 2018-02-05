Specifications for Client and Backend Communication
===
In this document *client* is referred to the Angular implementation in `./client` which provides the GUI (HTML/CSS/TypeScript) implementation. The *backend*  is referred to the C++ implementation in `./backend`.

The communication between *client* and *backend* is done over the `./client/.../CefMessageRouterService.ts` which executes the pre-registered `window.cef.cefQuery` on the *client* side and the receiving router `m_router` in `./cefapp/AppHandler.hpp`. Several message handlers are installed in the router `m_router` for the backend, in this case the `backend/ExecututionGraphBackend.cpp`, which provides all functionality for reading, storing, manipulating and executing a graph consisting of `LogicNodes`.

![Specs](client-backend.svg "Specs")

The application registers a file scheme handler factory `FileSchemeHandlerFactor` during `App::OnContextInitialized`. In this function, it also creates an application handler instance `AppHandler` which is forwarded to `CefBrowserHost::CreateBrowser`. 

During `AppHandler::OnAfterCreated()`, the backends are registered in `AppHandler::initializeBackends()`. So far an instance of `ExecutionGraphBackend` is registered in the `m_backendStorage`. 
Then, a browser-side router `m_router` of type `CefMessageRouterBrowserSide` is created and `BackendStorage::RegisterHandlersAtRouter(m_router)` is called which registers (by means of a factor `BackendMessageHandlerFactory`) all `BackendMessageHandlers` for all registered backends (in this case a `DummyBackendMsgHandler` for the single `ExecutionGraphBackend`). 

## Overview of MessageHandler Types
We need one message handlers for each of the following grouped functionalities:

-   Graph Creation Queries:
    * Add graph with id `graphId`
    * Delete graph with id `graphId`

-   Graph Info Queries :
    * Get info of all available socket types.
    * Get info of all addable node types.
    * Info query on node(s).
    * Info query on connection(s).

-   Serialization Functionalities:
    * Load a graph from a `.json` file into the backend and send a validated (possibly modified) graph summary info + the client `.json` back to the client such that it can visualize it.

    * Serialize a graph to a `.json` file. Corresponging of a serialization of the client data (gui visualization data) and backend data.

-   Graph Manipulation Queries:
    * Add/Remove node(s)
    * Add/Remove connection(s)

    * Set/Get node settings (`group id`, possible other custom node specific settings)

- Graph Execution Functionalities:
    * Run graph
    * Stop graph