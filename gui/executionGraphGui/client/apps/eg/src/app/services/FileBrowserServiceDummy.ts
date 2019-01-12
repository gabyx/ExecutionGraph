// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Thu Jan 10 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { Injectable, Inject } from '@angular/core';
import { VERBOSE_LOG_TOKEN } from '../tokens';
import { ILogger, LoggerFactory } from '@eg/logger';
import { FileBrowserService, FileInfo, DirectoryInfo, Permissions, isFile } from './FileBrowserService';

@Injectable()
export class FileBrowserServiceDummy extends FileBrowserService {
  private logger: ILogger;

  private root: DirectoryInfo = {
    path: './',
    name: '',
    size: 10,
    modified: new Date(),
    isFile: false,
    permissions: Permissions.OwnerReadWrite,
    files: [
      {
        path: './FileA',
        name: 'FileA',
        size: 4,
        modified: new Date(),
        isFile: true,
        permissions: Permissions.OwnerReadWrite
      },
      {
        path: './FileB',
        name: 'FileB',
        size: 4,
        modified: new Date(),
        isFile: true,
        permissions: Permissions.OwnerReadWrite
      }
    ],
    directories: [
      {
        path: './DirB',
        name: 'DirB',
        size: 4,
        modified: new Date(),
        isFile: false,
        permissions: Permissions.OwnerReadWrite,
        files: [
          {
            path: '/DirB/FileC',
            name: 'FileC',
            size: 4,
            modified: new Date(),
            isFile: true,
            permissions: Permissions.OwnerReadWrite
          }
        ],
        directories: []
      },
      {
        path: './DirC',
        name: 'DirC',
        size: 4,
        modified: new Date(),
        isFile: false,
        permissions: Permissions.OwnerReadWrite,
        files: [],
        directories: []
      }
    ]
  };

  constructor(loggerFactory: LoggerFactory, @Inject(VERBOSE_LOG_TOKEN) private readonly verboseResponseLog = true) {
    super();
    this.logger = loggerFactory.create('FileBrowserServiceDummy');
  }

  public async browse(path: string): Promise<FileInfo | DirectoryInfo> {
    const check = async (list: DirectoryInfo[] | FileInfo[], part: string): Promise<DirectoryInfo | FileInfo> => {
      for (let f of list) {
        this.logger.debug(`Checking: '${f.name}'`);
        if (part === f.name) {
          return f;
        }
      }
      throw 'Not in files!';
    };

    const getFile = async (current: FileInfo, part: string): Promise<FileInfo> => {
      if (current.name === part) {
        return current;
      }
      throw 'Not found';
    };

    const getDir = async (parent: DirectoryInfo, part: string) => {
      return check(parent.files, part).catch(e => check(parent.directories, part));
    };

    return new Promise<FileInfo | DirectoryInfo>(async resolve => {
      const s = path
        .split('/')
        .filter(s => s.length > 0)
        .slice(1);

      this.logger.debug(`Browse:  [${s}] length: ${s.length}`);

      let r: FileInfo | DirectoryInfo = this.root;

      for (let part of s) {
        this.logger.debug(`Part: ${part}`);
        if (!isFile(r)) {
          r = await getDir(r, part);
        } else {
          r = await getFile(r, part);
        }
      }
      resolve(r);
    }).catch(err => {
      console.log(err);
      throw `Path '${path}' not found!`;
    });
  }
}
