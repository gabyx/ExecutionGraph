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
import Application from '../common/App';
import { ArgumentParser } from 'argparse';
//require('source-map-support').install();

function parseArgs(): any {
  const parser = new ArgumentParser({
    version: '0.0.1',
    addHelp: true,
    description: 'ExecutionGraphGui'
  });
  parser.addArgument(['-c', '--clientSourcePath'], {
    help: 'The source path to the client app.',
    defaultValue: `${__dirname}/../client/apps/eg`
  });
  return parser.parseArgs(process.argv.splice(2));
}

const args = parseArgs();
const myApp = new Application(args, app, BrowserWindow);
myApp.run();
