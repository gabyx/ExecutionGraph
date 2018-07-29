import { Component, ElementRef, Input } from '@angular/core';

@Component({
  selector: 'ngcs-port',
  templateUrl: './port.component.html',
  styleUrls: ['./port.component.scss']
})
export class PortComponent {

  @Input()
  public id: string;
  
  constructor(public element: ElementRef) {
  }

}
