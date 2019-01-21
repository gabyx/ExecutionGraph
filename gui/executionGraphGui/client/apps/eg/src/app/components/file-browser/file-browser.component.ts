import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import { Store } from '@ngrx/store';
import { AppState } from '../../+state/reducers/app.reducers';
import { FileBrowserService } from '../../services';
import { DirectoryInfo, isFile, FileInfo, PathInfo } from '../../services/FileBrowserService';
import { ILogger, LoggerFactory } from '@eg/logger';
import { MatDialog } from '@angular/material';
import { ConfirmationDialogComponent } from '../confirmation-dialog/confirmation-dialog.component';

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
    readonly loggerFactory: LoggerFactory,
    private dialog: MatDialog
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
        if (d.path === this.rootPath) {
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
        throw new Error(`Cannot browse to a file: ${pathInfo.path}`);
      }
    });
  }

  public notEmpty(dir: DirectoryInfo) {
    return !dir.explored || (dir.files.length > 0 || dir.directories.length > 0);
  }

  public openFile(file: FileInfo) {
    this.logger.debug(`Opening file '${file.path}'`);
  }

  public deleteConfirm(path: FileInfo | DirectoryInfo) {
    const dialogRef = this.dialog.open(ConfirmationDialogComponent, {
      minWidth: '10%',
      data: {
        title: 'Delete',
        question: `Do you really want to delete the path '${path.path}' ?`
      }
    });
    dialogRef.afterClosed().subscribe(result => {
      if (result) {
        this.deleteFile(path);
      }
    });
  }

  public isFileOpenable(file: FileInfo) {
    return file.name.endsWith('.eg');
  }

  private deleteFile(path: FileInfo | DirectoryInfo) {
    if (this.isFileOpenable(path)) {
      this.logger.debug(`Deleting path '${path.path}'`);
    } else {
      this.logger.error('Programming Error!');
      throw new Error('Error!');
    }
  }
}
