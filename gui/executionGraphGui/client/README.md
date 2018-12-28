# Eg

This project was generated with [Angular CLI](https://github.com/angular/angular-cli) using [Nrwl Nx](https://nrwl.io/nx).

## Build commands
The following table illustrates the various build commands defined in `package.json`:
| Command | Description |
| ------- | ----------- |
| `npm run serve` | **Client App Sim**: Runs the angular build (in memory) and provides a development server under `http://localhost:4200`. This build uses all *dummy* services to simulate the backend.
| `npm run serve-dev` | **Client App Dev**: Runs the angular build (in memory) and provides a development server under `http://localhost:4200`. This build uses no *dummy* services, meaning it tries to connect to the backend.
| `npm run build` | **Client App Sim**:  Runs the angular build which stores the output in `./dist`. This build uses all *dummy* services to simulate the backend.
| `npm run build-dev` | **Client App Dev**:  Runs the angular build which stores the output in `./dist`. This build uses no *dummy* services, meaning it tries to connect to the backend.
| `npm run build-prod` | **Client App Prod**: Runs the angular build which stores the output in `./dist`. This build uses no *dummy* services, meaning it tries to connect to the backend.

| `npm run electron:serve` | **Native App Serve**: Serves an electron app and uses what ever configuration you build into the `dist` folder.
| `npm run electron:build` | **Native App Build**: Builds an electron app and uses what ever configuration you build into the `dist` folder.
| `npm run electorn:package` | **Native App Package**: Packages an electron app and uses what ever configuration you build into the `dist` folder.
| `npm run electron:serve:local-dev` | **Native App Serve + Local-Dev**: Serves an electron app and also serves a `developement` build (no dummy services) under `http://localhost:4200`.
| `npm run native:package` | **Native App Prod**: Builds and packages a full production version of the electron app.

## Running unit tests

Run `ng test` to execute the unit tests via [Karma](https://karma-runner.github.io).

## Running end-to-end tests

Run `ng e2e` to execute the end-to-end tests via [Protractor](http://www.protractortest.org/).
Before running the tests make sure you are serving the app via `ng serve`.

## Further help

To get more help on the Angular CLI use `ng help` or go check out the [Angular CLI README](https://github.com/angular/angular-cli/blob/master/README.md).
