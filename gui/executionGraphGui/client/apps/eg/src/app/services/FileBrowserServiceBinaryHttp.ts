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
import { flatbuffers } from 'flatbuffers';
import { ILogger, LoggerFactory } from '@eg/logger';
import { BinaryHttpRouterService } from './BinaryHttpRouterService';
import { FileBrowserService, FileInfo, DirectoryInfo, sz } from './FileBrowserService';
import { isDefined } from '@eg/common/src';

@Injectable()
export class FileBrowserServiceBinaryHttp extends FileBrowserService {
  private logger: ILogger;

  constructor(
    loggerFactory: LoggerFactory,
    private readonly binaryRouter: BinaryHttpRouterService,
    @Inject(VERBOSE_LOG_TOKEN) private readonly verboseResponseLog = true
  ) {
    super();
    this.logger = loggerFactory.create('FileBrowserServiceBinaryHttp');
  }

  /**
   * Traverse the path info tree and converting it.
   *
   * @private
   * @param {sz.BrowseResponse} response
   * @returns
   * @memberof FileBrowserServiceBinaryHttp
   */
  private convert(response: sz.BrowseResponse) {
    interface Entry {
      from: sz.PathInfo;
      to?: FileInfo | DirectoryInfo;
    }

    const root: Entry = { from: response.info() };
    const stack: Array<Entry> = [root];

    const convertFile = (from: sz.PathInfo, to?: FileInfo): FileInfo => {
      return Object.assign(!isDefined(to) ? <FileInfo>{} : to, {
        path: from.path(),
        permissions: from.permissions(),
        modified: from.modified(),
        size: from.size().toFloat64(),
        isFile: true
      });
    };

    const convertDirectory = (from: sz.PathInfo, toIn: DirectoryInfo): DirectoryInfo => {
      const to = Object.assign(!isDefined(toIn) ? <DirectoryInfo>{} : toIn, {
        path: from.path(),
        name: from.name(),
        permissions: from.permissions(),
        modified: from.modified(),
        size: from.size().toFloat64(),
        isFile: false
      });

      const files = from.files.bind(from);
      let l = from.filesLength();
      if (l > 0) {
        to.files = new Array<FileInfo>();
      }
      for (let i = 0; i < l; ++i) {
        to.files.push(convertFile(files(i)));
      }

      const dirs = from.directories.bind(from);
      l = from.directoriesLength();

      if (l > 0) {
        to.directories = new Array<DirectoryInfo>();
      }
      for (let i = 0; i < l; ++i) {
        // fill into stack
        const toDir = <DirectoryInfo>{};
        to.directories.push(toDir);
        stack.push({ from: dirs(i), to: toDir });
      }

      return to;
    };

    // Traverse stack.
    while (stack.length) {
      const e = stack.pop();
      if (e.from.isFile()) {
        e.to = convertFile(e.from);
      } else {
        e.to = convertDirectory(e.from, e.to);
      }
    }

    return root.to;
  }

  public async browse(path: string): Promise<FileInfo | DirectoryInfo> {
    const builder = new flatbuffers.Builder(256);
    const pathOff = builder.createString(path);

    sz.BrowseRequest.startBrowseRequest(builder);
    sz.BrowseRequest.addPath(builder, pathOff);
    let off = sz.BrowseRequest.endBrowseRequest(builder);
    builder.finish(off);
    const requestPayload = builder.asUint8Array();

    // Send the request
    const r = await this.binaryRouter.post('files/browse', requestPayload);
    const buf = new flatbuffers.ByteBuffer(r);
    const response = sz.BrowseResponse.getRootAsBrowseResponse(buf);
    const pathInfo = this.convert(response);

    if (this.verboseResponseLog) {
      this.logger.debug(`Browse: ${JSON.stringify(pathInfo, null, 4)}`);
    }

    return pathInfo;
  }
}
