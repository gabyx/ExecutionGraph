import { Component, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { AppState } from '../../+state/reducers/app.reducers';
import { FileBrowserService } from '../../services';
import { DirectoryInfo, isFile, FileInfo } from '../../services/FileBrowserService';

@Component({
  selector: 'eg-file-browser',
  templateUrl: './file-browser.component.html',
  styleUrls: ['./file-browser.component.scss']
})
export class FileBrowserComponent implements OnInit {

  isLoading = false;

  currentDirectory: DirectoryInfo = null;
  
  constructor(private store: Store<AppState>, private browser: FileBrowserService) {}

  ngOnInit() {
    this.openDirectory('');
  }

  openDirectory(path: string) {
    this.isLoading = true;

    this.browser.browse(path).then(pathInfo => {

      if(!isFile(pathInfo)) {
        this.currentDirectory = pathInfo;
      }
      else {
        console.error(`Cannot browse to a file: ${pathInfo.path}`);
      }

      this.isLoading = false;
    });
  }

  openFile(file: FileInfo) {
  }
}
