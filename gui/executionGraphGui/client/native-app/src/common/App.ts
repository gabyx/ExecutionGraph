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

import { BrowserWindow, App } from 'electron';
import * as path from 'path';

export default class Application {
  private mainWindow: BrowserWindow | null = null;

  constructor(private readonly args: any, private app: App, private BrowserWindowCls: typeof BrowserWindow) {
    // we pass the App object and the
    // BrowserWindow into this function
    // so this class has no dependencies. This
    // makes the code easier to write tests for
  }

  public run() {
    this.app.on('window-all-closed', this.onWindowAllClosed.bind(this));
    this.app.on('ready', this.onReady.bind(this));
    this.app.on('activate', () => {
      // On OS X it"s common to re-create a window in the app when the
      // dock icon is clicked and there are no other windows open.
      if (this.mainWindow === null) {
        this.mainWindow = this.createWindow(this.args['clientSourcePath']);
      }
    });
  }

  private createWindow(clientSourcePath: string) {
    // Create the browser window.
    let window: BrowserWindow | null = new this.BrowserWindowCls({
      width: 1680,
      height: 1050,
      backgroundColor: '#ffffff',
      icon: `file://${__dirname}/resources/icon.png`
    });

    // Load the index.html of the app.
    clientSourcePath = clientSourcePath ? clientSourcePath.trim() : '';
    if (!clientSourcePath.endsWith('index.html')) {
      clientSourcePath = path.join(clientSourcePath, 'index.html');
    }
    if (!clientSourcePath.startsWith('http://') && !clientSourcePath.startsWith('https://')) {
      if (!path.isAbsolute(clientSourcePath)) {
        clientSourcePath = path.join(__dirname, clientSourcePath);
      }
      clientSourcePath = 'file://' + clientSourcePath;
    }

    console.info(`Load url: '${clientSourcePath}'`);
    window.loadURL(clientSourcePath);

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

  private onWindowAllClosed() {
    if (process.platform !== 'darwin') {
      this.app.quit();
    }
  }

  private onClose() {
    // Dereference the window object.
    this.mainWindow = null;
  }

  private onReady() {
    this.mainWindow = this.createWindow(this.args['clientSourcePath']);
    this.mainWindow.on('closed', this.onClose.bind(this));
  }
}
