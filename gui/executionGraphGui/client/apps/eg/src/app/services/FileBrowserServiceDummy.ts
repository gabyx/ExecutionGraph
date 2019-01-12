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
import { FileBrowserService, FileInfo, DirectoryInfo, sz, Permissions } from './FileBrowserService';

@Injectable()
export class FileBrowserServiceDummy extends FileBrowserService {
  private logger: ILogger;

  constructor(loggerFactory: LoggerFactory, @Inject(VERBOSE_LOG_TOKEN) private readonly verboseResponseLog = true) {
    super();
    this.logger = loggerFactory.create('FileBrowserServiceDummy');
  }

  public async browse(path: string): Promise<FileInfo | DirectoryInfo> {
    return {
      path: 'FolderA',
      name: 'FolderA',
      size: 10,
      modified: new Date(),
      isFile: false,
      permissions: Permissions.OwnerReadWrite,
      files: [
        {
          path: 'FolderA/FileA',
          name: 'FileA',
          size: 4,
          modified: new Date(),
          isFile: true,
          permissions: Permissions.OwnerReadWrite
        },
        {
          path: 'FolderA/FileB',
          name: 'FileB',
          size: 4,
          modified: new Date(),
          isFile: true,
          permissions: Permissions.OwnerReadWrite
        }
      ],
      directories: [
        {
          path: 'FolderA/DirB',
          name: 'DirB',
          size: 4,
          modified: new Date(),
          isFile: false,
          permissions: Permissions.OwnerReadWrite,
          files: [
            {
              path: 'FolderA/DirB/FileC',
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
          path: 'FolderA/DirC',
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
  }
}
