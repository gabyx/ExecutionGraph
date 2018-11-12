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

import * as path from 'path';
import { app, BrowserWindow } from 'electron';
import { ArgumentParser } from 'argparse';
import Application from '../common/App';

function parseArgs(): any {
  const parser = new ArgumentParser({
    version: '0.0.1',
    addHelp: true,
    prog: 'ExecutionGraphGui',
    description: 'GUI of the execution graph'
  });
  const defaultClientSourcePath = path.join(__dirname, `client/apps/eg`);
  console.log(`Defaulting client source path to ${defaultClientSourcePath}`);

  parser.addArgument(['-c', '--clientSourcePath'], {
    help: 'The source path to the client app.',
    defaultValue: defaultClientSourcePath
  });
  console.log(process.argv)
  return parser.parseArgs(process.argv.slice(2));
}

const args = parseArgs();
const myApp = new Application(args, app, BrowserWindow);
myApp.run();
