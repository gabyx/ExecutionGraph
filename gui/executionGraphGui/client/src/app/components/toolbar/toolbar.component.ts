import { Component, OnInit } from '@angular/core';

import { ExecutionService } from '../../services/ExecutionService';

@Component({
  selector: 'eg-toolbar',
  templateUrl: './toolbar.component.html',
  styleUrls: ['./toolbar.component.scss']
})
export class ToolbarComponent implements OnInit {

  public testResponse: any;

  constructor(private readonly executionService: ExecutionService) { }

  ngOnInit() {
  }

  public test() {
    console.log("Testing");
    this.executionService.execute()
      .then(result => {
        this.testResponse = result;
      }, error => {
        this.testResponse = error;
      })
  }
}
