# Eg

This project was generated with [Angular CLI](https://github.com/angular/angular-cli) using [Nrwl Nx](https://nrwl.io/nx).

## Build commands
The following table illustrates the various build commands defined in `package.json`:
| Command | Description |
| ------- | ----------- |
| `npm run start` | **Client App Simulation Build**: Runs the angular build (in memory) and provides a development server under `http://localhost:4200`. This build uses all *dummy* services to simulate the backend.
| `npm run build-dev` | **Client Development Build**:  Runs the angular build which stores the output in `./dist`. This build uses no *dummy* services, meaning it tries to connect to the backend.
| `npm run build-prod` | **Client App Production Build**: Runs the angular build which stores the output in `./dist`. This build uses no *dummy* services, meaning it tries to connect to the backend.
| `npm run build-prod` | **Client App Production Build**: Runs the angular build *continuously* on file changes and stores the output in `./dist`. This build uses no *dummy* services, meaning it tries to connect to the backend.
| `npm run start-native` | **Native App Simulation Build**: Runs the angular build and stores the output in `./native-app/dist` for the electron gui application and starts the application afterwards. This build uses all *dummy* services to simulate the backend.
| `npm run build-native` | **Native App Simulation Build**: Runs the angular build and stores the output in `./native-app/dist` for the electron gui application. This build uses no *dummy* services, meaning it tries to connect to the backend.
| `npm run build-dev-native` | **Native App Development Build**: Runs the angular build and stores the output in `./native-app/dist` for the electron gui application. This build uses no *dummy* services, meaning it tries to connect to the backend.
| `npm run build-prod-native` | **Native App Production Build**: Runs the angular build and stores the output in `./native-app/dist` for the electron gui application. This build uses no *dummy* services, meaning it tries to connect to the backend.

## Running unit tests

Run `ng test` to execute the unit tests via [Karma](https://karma-runner.github.io).

## Running end-to-end tests

Run `ng e2e` to execute the end-to-end tests via [Protractor](http://www.protractortest.org/).
Before running the tests make sure you are serving the app via `ng serve`.

## Further help

To get more help on the Angular CLI use `ng help` or go check out the [Angular CLI README](https://github.com/angular/angular-cli/blob/master/README.md).
