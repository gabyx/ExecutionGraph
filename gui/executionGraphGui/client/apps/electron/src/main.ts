import { app, BrowserWindow, screen } from 'electron';
import { execFile } from 'child_process';
import * as path from 'path';
import * as url from 'url';
import * as fs from 'fs';

let win: BrowserWindow = null;

const args = process.argv.slice(1);
const serve = args.some(val => val === '--serve');

function createWindow() {
  const electronScreen = screen;
  const size = electronScreen.getPrimaryDisplay().workAreaSize;

  // Create the browser window.
  win = new BrowserWindow({
    x: 0,
    y: 0,
    width: size.width,
    height: size.height,
    webPreferences: {
      nodeIntegration: false,
      webSecurity: false
    }
  });

  win.center();

  if (serve) {
    const loadUrl = 'http://localhost:4200';
    const projectPath = path.join(__dirname, '..');
    console.log(`Serving form ${projectPath}`);

    // Reload initial page to not get mixed up with client side routing
    win.webContents.on('devtools-reload-page', event => {
      win.loadURL(loadUrl);
    });

    // Hijack this event, which is triggered by the HMR feature of Angular CLI
    // This will reload the index instead of trying to refresh the page at the current route.
    win.webContents.on('will-navigate', (event, navigationUrl) => {
      console.log(`will-navigate url to '${navigationUrl}'`);
      event.preventDefault();
      win.loadURL(loadUrl);
    });

    win.loadURL(loadUrl);
  } else {
    win.loadURL(
      url.format({
        pathname: path.join(__dirname, 'apps/eg/index.html'),
        protocol: 'file:',
        slashes: true
      })
    );
  }

  win.webContents.openDevTools();

  // Start Execution Graph Server
  const serverExec = path.join(__dirname, 'ExecutionGraphServer');
  if (fs.existsSync(serverExec)) {
    console.log(`Starting up server: '${serverExec}'`);
    execFile(serverExec, (err, data) => {
      if (err) {
        console.error(err);
        return;
      }
      console.log(data.toString());
    });
  }

  // Emitted when the window is closed.
  win.on('closed', () => {
    // Dereference the window object, usually you would store window
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    win = null;
  });
}

try {
  // This method will be called when Electron has finished
  // initialization and is ready to create browser windows.
  // Some APIs can only be used after this event occurs.
  app.on('ready', createWindow);

  // Quit when all windows are closed.
  app.on('window-all-closed', () => {
    // On OS X it is common for applications and their menu bar
    // to stay active until the user quits explicitly with Cmd + Q
    if (process.platform !== 'darwin') {
      app.quit();
    }
  });

  app.on('activate', () => {
    // On OS X it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (win === null) {
      createWindow();
    }
  });
} catch (e) {
  // Catch Error
  // throw e;
}
