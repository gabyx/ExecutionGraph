import { Component, OnInit, EventEmitter, Output, ViewChild, ElementRef } from '@angular/core';
import { of } from 'rxjs';
import { delay, first } from 'rxjs/operators';

@Component({
  selector: 'file-browser',
  templateUrl: './file-browser.component.html'
})
export class FileBrowserComponent implements OnInit {
  constructor() {}
  @ViewChild('inputElement')
  input: ElementRef;
  @Output()
  path = new EventEmitter<string>();

  /**
   * Opens the file dialog with a delay
   * to finish ongoing animations.
   *
   * @param {number} [delayT=200]
   * @memberof FileBrowserComponent
   */
  public async open(delayT: number = 200) {
    of(null)
      .pipe(
        delay(delayT),
        first()
      )
      .subscribe(() => {
        this.input.nativeElement.click();
        this.path.emit(this.input.nativeElement.value);
      });
  }

  public ngOnInit() {}
}
