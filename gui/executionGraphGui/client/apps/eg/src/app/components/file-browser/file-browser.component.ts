import { Component, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { AppState } from '../../+state/reducers/app.reducers';
import { FileBrowserService } from '../../services';
import { DirectoryInfo, isFile, FileInfo } from '../../services/FileBrowserService';
import { ILogger, LoggerFactory } from '@eg/logger/src';

@Component({
  selector: 'eg-file-browser',
  templateUrl: './file-browser.component.html',
  styleUrls: ['./file-browser.component.scss']
})
export class FileBrowserComponent implements OnInit {
  private logger: ILogger;
  private initPath = '.';

  private rootPath: string;
  private atRoot: boolean;
  private isLoading = false;
  private parentPaths: string[] = [];

  private currentDirectory: DirectoryInfo = null;

  constructor(
    private store: Store<AppState>,
    private browser: FileBrowserService,
    readonly loggerFactory: LoggerFactory
  ) {
    this.logger = loggerFactory.create('FileBrowserComponent');
  }

  ngOnInit() {
    this.openRoot();
  }

  private openRoot() {
    this.reset();
    this.loadDirectory(this.initPath).then(d => {
      this.rootPath = d.path;
      this.atRoot = true;
      this.currentDirectory = d;
    });
  }

  private reset() {
    this.atRoot = true;
    this.rootPath = undefined;
    this.parentPaths = [];
  }

  public openDirectory(dir: DirectoryInfo) {
    if (dir) {
      this.loadDirectory(dir.path).then(d => {
        this.parentPaths.push(this.currentDirectory.path);
        this.currentDirectory = d;
        this.atRoot = false;
      });
    }
  }

  public openParentDirectory() {
    const p = this.parentPaths[this.parentPaths.length - 1];
    if (p) {
      this.loadDirectory(p).then(d => {
        if (d.path == this.rootPath) {
          this.atRoot = true;
        }
        this.parentPaths.pop();
        this.currentDirectory = d;
      });
    }
  }

  private async loadDirectory(path: string): Promise<DirectoryInfo> {
    this.isLoading = true;
    this.logger.debug(`loadDirectory: '${path}'`);
    return this.browser.browse(path).then(pathInfo => {
      if (!isFile(pathInfo)) {
        this.isLoading = false;
        return pathInfo;
      } else {
        throw `Cannot browse to a file: ${pathInfo.path}`;
      }
    });
  }

  public notEmpty(dir: DirectoryInfo) {
    return dir.files.length > 0 || dir.directories.length > 0;
  }

  openFile(file: FileInfo) {}
}
