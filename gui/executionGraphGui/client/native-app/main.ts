// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Nov 10 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

// This is the electron main.ts to start the client application.

import { app, BrowserWindow } from 'electron';
import { ArgumentParser } from 'argparse';
import * as path from 'path';

/* Parsing command line arguments */
function parseArgs(): any {
  let parser = new ArgumentParser({
    version: '0.0.1',
    addHelp: true,
    description: 'ExecutionGraphGui'
  });
  parser.addArgument(['-c', '--clientSourcePath'], {
    help: 'The source path to the client application.',
    defaultValue: `${__dirname}/client/app/eg`
  });
  return parser.parseArgs();
}

function createWindow(clientSourcePath: string) {
  // Create the browser window.
  let window = new BrowserWindow({
    width: 1680,
    height: 1050,
    backgroundColor: '#ffffff',
    icon: `file://${__dirname}/resources/icon.png`
  });

  // Load the index.html of the app.
  clientSourcePath = clientSourcePath.trim();
  if (!clientSourcePath.endsWith('index.html')) {
    clientSourcePath = path.join(clientSourcePath, 'index.html');
  }
  if (clientSourcePath.startsWith('file://') || clientSourcePath.startsWith('http://')) {
    console.info(`Load url: '${clientSourcePath}'`);
    window.loadURL(clientSourcePath);
  } else {
    if (!path.isAbsolute(clientSourcePath)) {
      clientSourcePath = path.join(__dirname, clientSourcePath);
    }
    console.info(`Load file: '${clientSourcePath}'`);
    window.loadFile(clientSourcePath);
  }

  // Open the DevTools.
  window.webContents.openDevTools();

  // Emitted when the window is closed.
  window.on('closed', () => {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    window = null;
  });
  return window;
}

// Parse the application arguments.
const cliArguments = parseArgs();
console.log(cliArguments);

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
let mainWindow: Electron.BrowserWindow;
app.on('ready', () => (mainWindow = createWindow(cliArguments['clientSourcePath'])));

// Quit when all windows are closed.
app.on('window-all-closed', () => {
  // On OS X it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  // On OS X it"s common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) {
    mainWindow = createWindow(cliArguments['clientSourcePath']);
  }
});

// In this file you can include the rest of your app"s specific main process
// code. You can also put them in separate files and require them here.
