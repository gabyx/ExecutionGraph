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
import * as model from '../model';
import * as conversions from './Conversions';
import { FileBrowserService, FileInfo, DirectoryInfo, sz } from './FileBrowserService';
import { pipe } from 'rxjs';
import { parseIntAutoRadix } from '@angular/common/src/i18n/format_number';
import { parseTimelineCommand } from '@angular/animations/browser/src/render/shared';
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
    const stack = new Array<Entry>(root);

    const convertDirectory = (from: sz.PathInfo, toIn?: DirectoryInfo): DirectoryInfo => {
      const to = Object.assign(
        {
          path: from.path(),
          permissions: from.permissions(),
          modified: from.modified(),
          size: from.size().toFloat64(),
          isFile: false
        },
        toIn === undefined ? <DirectoryInfo>{} : toIn
      );

      const files = from.files;
      let l = files.length;
      if (l > 0) {
        to.files = new Array<FileInfo>();
      }
      for (let i = 0; i < l; ++i) {
        const p = files(i);
        to.files.push(convertFile(p));
      }

      const dirs = from.directories;
      l = files.length;
      if (l > 0) {
        to.directories = new Array<DirectoryInfo>(l);
      }
      for (let i = 0; i < l; ++i) {
        const p = dirs(i);
        // fill into stack
        stack.push({ from: p, to: to.directories[i] });
      }

      return to;
    };

    const convertFile = (from: sz.PathInfo, to?: FileInfo): FileInfo => {
      return Object.assign(
        {
          path: from.path(),
          permissions: from.permissions(),
          modified: from.modified(),
          size: from.size().toFloat64(),
          isFile: true
        },
        to === undefined ? <FileInfo>{} : to
      );
    };

    // Traverse stack.
    while (stack.length) {
      const e = stack.pop();
      if (e.from.isFile()) {
        e.to = convertFile(e.from);
      } else {
        e.to = convertDirectory(e.from);
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
      this.logger.debug(`Browse: ${JSON.stringify(pathInfo)}`);
    }

    return pathInfo;
  }
}
