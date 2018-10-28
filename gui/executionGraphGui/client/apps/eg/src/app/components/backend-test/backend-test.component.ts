import { Component, OnInit } from '@angular/core';
import { TestService } from '../../services/TestService';

@Component({
  selector: 'eg-backend-test',
  templateUrl: './backend-test.component.html',
  styleUrls: ['./backend-test.component.css']
})
export class BackendTestComponent implements OnInit {

  public testResponse: any;

  constructor(private readonly testService: TestService) {
  }

  ngOnInit() { }

  public test() {
    this.testService.testAddRemove();
  }
}
