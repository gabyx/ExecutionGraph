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

  public async browse(path: string): Promise<FileInfo | DirectoryInfo> {
    const builder = new flatbuffers.Builder(256);
    const pathOff = builder.createString(path);

    sz.BrowseRequest.startBrowseRequest(builder);
    sz.BrowseRequest.addPath(builder, pathOff);
    const off = sz.BrowseRequest.endBrowseRequest(builder);
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

  private convert(response: sz.BrowseResponse): Promise<DirectoryInfo | FileInfo> {
    const pathInfo = response.info();
    return pathInfo.isFile() ? this.convertFileInfo(pathInfo) : this.convertDirectory(pathInfo);
  }

  private async convertDirectory(from: sz.PathInfo): Promise<DirectoryInfo> {
    const directory = await this.convertDirectoryInfo(from);

    for (let fIdx = 0; fIdx < from.filesLength(); ++fIdx) {
      directory.files.push(await this.convertFileInfo(from.files.bind(from)(fIdx)));
    }
    for (let dIdx = 0; dIdx < from.directoriesLength(); ++dIdx) {
      directory.directories.push(await this.convertDirectory(from.directories.bind(from)(dIdx)));
    }

    return directory;
  }

  private convertFileInfo(from: sz.PathInfo): Promise<FileInfo> {
    return new Promise((resolve, reject) => {
      resolve({
        path: from.path(),
        name: from.name(),
        permissions: from.permissions(),
        modified: this.convertDate(from.modified()),
        size: from.size().toFloat64(),
        isFile: true
      });
    });
  }

  private convertDirectoryInfo(from: sz.PathInfo): Promise<DirectoryInfo> {
    return new Promise((resolve, reject) => {
      resolve({
        path: from.path(),
        name: from.name(),
        permissions: from.permissions(),
        modified: this.convertDate(from.modified()),
        size: from.size().toFloat64(),
        isFile: false,
        directories: [],
        files: []
      });
    });
  }

  private convertDate(from: sz.Date): Date {
    return new Date(from.year(), from.month(), from.day(), from.hour(), from.min(), from.sec(), 0);
  }

}
