import { Directive, OnInit, OnDestroy } from '@angular/core';

@Directive({
  selector: '[fileBrowser]'
})
export class FileBrowserDirective implements OnInit, OnDestroy {
  public ngOnInit() {}
  public ngOnDestroy() {}
}
