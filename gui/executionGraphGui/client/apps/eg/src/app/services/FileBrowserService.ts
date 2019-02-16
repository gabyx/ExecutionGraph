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

import { FileBrowserMessages } from '@eg/messages';
export import sz = FileBrowserMessages;

export enum Permissions {
  None,
  OwnerRead,
  OwnerWrite,
  OwnerReadWrite
}

export interface PathInfo {
  path: string;
  name: string;
  permissions: Permissions;
  size: number;
  modified: Date;
  isFile: boolean;
}

export type FileInfo = PathInfo;

export interface DirectoryInfo extends PathInfo {
  files: FileInfo[];
  directories: DirectoryInfo[];
  explored: boolean; //! Denoting if this folder has been explored or recursion stopped here.
}

export function isFile(p: FileInfo | DirectoryInfo): p is FileInfo {
  return p.isFile;
}

export function isDirectory(p: FileInfo | DirectoryInfo): p is DirectoryInfo {
  return !p.isFile;
}

export abstract class FileBrowserService {
  public abstract async getPathInfo(path: string): Promise<DirectoryInfo | FileInfo>;
  public abstract async getPathInfo(path: string): Promise<DirectoryInfo | FileInfo>;
}
