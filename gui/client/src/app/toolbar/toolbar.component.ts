import { Component, OnInit } from '@angular/core';

import { CefMessageRouterService } from '../services/CefMessageRouterService'

@Component({
  selector: 'eg-toolbar',
  templateUrl: './toolbar.component.html',
  styleUrls: ['./toolbar.component.scss']
})
export class ToolbarComponent implements OnInit {

  public testResult: any;

  constructor(private readonly messageRouter: CefMessageRouterService) { }

  ngOnInit() {
  }

  public test() {
    console.log("Testing");
    this.messageRouter.execute<string>('test', {id: 123})
      .then(result => {
        this.testResult = result;
      }, error => {
        this.testResult = error;
      })
  }
}
