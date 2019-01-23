import { Component, OnInit, ViewChild, ElementRef, EventEmitter, Output, Input } from '@angular/core';
import { Store } from '@ngrx/store';
import { AppState } from '../../+state/reducers/app.reducers';
import { FileBrowserService } from '../../services';
import { DirectoryInfo, isFile, FileInfo, PathInfo } from '../../services/FileBrowserService';
import { ILogger, LoggerFactory } from '@eg/logger';
import { MatDialog } from '@angular/material';
import { ConfirmationDialogComponent } from '../confirmation-dialog/confirmation-dialog.component';
import { assertThat } from '../../general/Assert';

export enum FileBrowserMode {
  Open = 'open',
  Save = 'save'
}

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

  @Input('mode') mode: string = FileBrowserMode.Open;
  @Input('allowDelete') allowDelete: boolean = true;

  @Output('fileActionSave') fileActionSave: EventEmitter<string>;
  @Output('fileActionOpen') fileActionOpen: EventEmitter<string>;
  @Output('fileActionDelete') fileActionDelete: EventEmitter<string>;

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

  private openParentDirectory() {
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
    assertThat(this.mode === FileBrowserMode.Open, 'Programming Error!');
    this.logger.debug(`Opening file '${file.path}'`);
    this.fileActionOpen.emit(file.path);
  }

  private checkOverwrite(path: string) {
    for (const p of this.currentDirectory.files) {
      if (p.path.replace('//', '/') === path.replace('//', '/')) {
        return true;
      }
    }
  }

  public saveFileName(fileName: string) {
    assertThat(this.mode === FileBrowserMode.Save && this.isFileNameCorrect(fileName), 'Programming Error!');
    const path = (this.currentDirectory.path + '/' + fileName).replace('//', '/');
    this.saveFile(path, true);
  }

  private deleteFile(path: FileInfo | DirectoryInfo) {
    assertThat(this.allowDelete && this.isFileOpenable(path), 'Programming Error!');
    this.logger.debug(`Deleting path '${path.path}'`);
    this.fileActionDelete.emit(path.path);
  }

  private deleteConfirm(path: FileInfo | DirectoryInfo) {
    assertThat(this.allowDelete, 'Programming Error!');
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

  private saveFile(path: string, checkOverwrite: boolean) {
    assertThat(this.mode === FileBrowserMode.Save, 'Programming Error!');

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
        this.fileActionSave.emit(path);
      }
    });
  }

  public isFileOpenable(file: FileInfo) {
    return file.name.endsWith('.eg');
  }

  public isFileNameCorrect(fileName: string) {
    // https://regex101.com/r/Owqjpb/1
    return fileName.match(/^(?<!\.)\w([^\\/]+)\.eg$/gm).length > 0;
  }
}
