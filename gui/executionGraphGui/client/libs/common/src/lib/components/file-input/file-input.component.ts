import { Component, EventEmitter, Output, ViewChild, ElementRef } from '@angular/core';
import { of } from 'rxjs';
import { delay, first } from 'rxjs/operators';

@Component({
  selector: 'eg-input-browser',
  templateUrl: './file-input.component.html'
})
export class FileInputComponent {

  /**
   * The user changed the selected file
   *
   * @memberof FileInputComponent
   */
  @Output() fileChanged = new EventEmitter<File>();

  @ViewChild('inputElement') private input: ElementRef;

  private get fileInputElement() : HTMLInputElement {
    return this.input.nativeElement;
  }

  /**
   * Opens the file dialog with a delay
   * to finish ongoing animations.
   *
   * @param {number} [delayT=200]
   * @memberof FileInputComponent
   */
  public open(delayT: number = 200) {
    of(null)
      .pipe(
        delay(delayT),
        first()
      )
      .subscribe(() => {
        this.fileInputElement.click();
      });
  }

  /**
   * Event handler for changes on the file Input element,
   * i.e. another file has been selected
   *
   * @param {Event} event
   * @memberof FileInputComponent
   */
  public onFileChanged(event: Event) {
    if (this.fileInputElement.files.length > 0)
    {
      // @todo cmonspqr -> gabyx: Is multi-selection allowed? if so, emit multiple files
      this.fileChanged.emit(this.fileInputElement.files[0]);
    }
  }
}
