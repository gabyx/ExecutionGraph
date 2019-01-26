import { Component, OnInit, ViewChild, ElementRef, EventEmitter, Output, Input } from '@angular/core';
import { Store } from '@ngrx/store';
import { AppState } from '../../+state/reducers/app.reducers';
import { FileBrowserService } from '../../services';
import { DirectoryInfo, isFile, FileInfo, PathInfo } from '../../services/FileBrowserService';
import { ILogger, LoggerFactory } from '@eg/logger';
import { MatDialog } from '@angular/material';
import { ConfirmationDialogComponent } from '../confirmation-dialog/confirmation-dialog.component';
import { assert } from '@eg/common';
import { FormGroup, FormControl, Validators } from '@angular/forms';
import { isNullOrUndefined } from 'util';

export enum FileBrowserMode {
  Open = 'open',
  Save = 'save'
}

export interface SaveAction {
  filePath: string;
  overwrite: boolean;
}

@Component({
  selector: 'eg-file-browser',
  templateUrl: './file-browser.component.html',
  styleUrls: ['./file-browser.component.scss']
})
export class FileBrowserComponent implements OnInit {
  @Input() mode: string = FileBrowserMode.Open;
  @Input() allowDelete: boolean = true;

  @Output() fileActionSave = new EventEmitter<SaveAction>();
  @Output() fileActionOpen = new EventEmitter<string>();
  @Output() fileActionDelete = new EventEmitter<string>();

  fileNameForm: FormGroup;
  currentDirectory: DirectoryInfo = null;
  isLoading = false;
  atRoot: boolean;

  private readonly logger: ILogger;

  private rootPath: string;
  private parentPaths: string[] = [];

  private readonly fileNameRegex = /^\w([^\\/ ])+\.eg$/m;

  constructor(
    private store: Store<AppState>,
    private browser: FileBrowserService,
    readonly loggerFactory: LoggerFactory,
    private dialog: MatDialog
  ) {
    this.logger = loggerFactory.create('FileBrowserComponent');
  }
  ngOnInit() {
    this.fileNameForm = new FormGroup({
      fileName: new FormControl('MyGraph.eg', [Validators.required, Validators.pattern(this.fileNameRegex)])
    });
    this.openRoot();
  }

  private openRoot() {
    this.reset();
    this.loadDirectory('.').then(d => {
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
    return this.browser.getPathInfo(path).then(pathInfo => {
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
    assert(this.mode === FileBrowserMode.Open, 'Programming Error!');
    this.logger.debug(`Opening file '${file.path}'`);
    this.fileActionOpen.emit(file.path);
  }

  public saveNewFile() {
    assert(this.mode === FileBrowserMode.Save, 'Ensure saving is only available in save Mode!');
    assert(this.fileNameForm.valid, 'Ensure valid file names in the UI!');
    assert(!isNullOrUndefined(this.currentDirectory), 'Directory must be known when saving a file');

    const fileName = this.fileNameForm.get('fileName').value;
    const path = `${this.currentDirectory.path}/${fileName}`.replace('//', '/');
    this.saveFile(path, true);
  }

  public deleteConfirm(path: FileInfo | DirectoryInfo) {
    assert(this.allowDelete, 'Programming Error!');
    const dialogRef = this.dialog.open(ConfirmationDialogComponent, {
      minWidth: '10%',
      data: {
        title: 'Delete?',
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
    assert(file.isFile, 'Programming Error!');
    return file.name.match(this.fileNameRegex) !== null;
  }

  private checkOverwrite(path: string) {
    for (const p of this.currentDirectory.files) {
      if (p.path.replace('//', '/') === path.replace('//', '/')) {
        return true;
      }
    }
  }

  private deleteFile(path: FileInfo | DirectoryInfo) {
    assert(this.allowDelete, 'Ensure deleting is only available when deleting is allowed');
    assert(this.isFileOpenable(path), 'Only openable files should be deletable!');
    this.logger.debug(`Deleting path '${path.path}'`);
    this.fileActionDelete.emit(path.path);
  }

  private saveFile(path: string, checkOverwrite: boolean) {
    assert(this.mode === FileBrowserMode.Save, 'Programming Error!');

    const showOverwrite = checkOverwrite ? this.checkOverwrite(path) : false;

    const dialogRef = this.dialog.open(ConfirmationDialogComponent, {
      minWidth: '10%',
      data: {
        title: showOverwrite ? 'Overwrite?' : 'Save?',
        question: showOverwrite ? `Do you really want to overwrite the path '${path}' ?` : `Saving to path: '${path}' ?`
      }
    });
    dialogRef.afterClosed().subscribe(result => {
      if (result) {
        this.logger.debug(`Save file '${path}'`);
        this.fileActionSave.emit({ filePath: path, overwrite: showOverwrite });
      }
    });
  }
}
