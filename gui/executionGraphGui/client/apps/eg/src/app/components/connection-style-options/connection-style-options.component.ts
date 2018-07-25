import { Component, OnInit, EventEmitter, Output, Input } from '@angular/core';
import { ConnectionDrawStyle, DirectConnectionDrawStyle, ManhattenConnectionDrawStyle, BezierConnectionDrawStyle } from '@eg/graph';

type ConnectionDrawStyleName = 'direct' | 'manhatten' | 'bezier';

const drawStyles = {
  'direct': new DirectConnectionDrawStyle(),
  'manhatten': new ManhattenConnectionDrawStyle(),
  'bezier': new BezierConnectionDrawStyle(),
}

@Component({
  selector: 'eg-connection-style-options',
  templateUrl: './connection-style-options.component.html',
  styleUrls: ['./connection-style-options.component.css']
})
export class ConnectionStyleOptionsComponent implements OnInit {

  @Output()
  connectionDrawStyleChanged = new EventEmitter<ConnectionDrawStyle>();
  @Input()
  connectionDrawStyle: ConnectionDrawStyle = new DirectConnectionDrawStyle();

  get drawStyle(): ConnectionDrawStyleName {
    return this.drawStyleName;
  }
  set drawStyle(drawStyleName: ConnectionDrawStyleName) {
    this.drawStyleName = drawStyleName;
    switch(this.drawStyleName)
    {
      case 'direct':
        this.connectionDrawStyle = new DirectConnectionDrawStyle();
        break;
      case 'manhatten':
        this.connectionDrawStyle = new ManhattenConnectionDrawStyle();
        break;
      case 'bezier':
        this.connectionDrawStyle = new BezierConnectionDrawStyle();
        break;
    }
    console.log(`[ConnectionStyleOptionsComponent] Draw Style changed to ${this.drawStyleName}`);
    this.connectionDrawStyleChanged.emit(this.connectionDrawStyle);
  }

  private drawStyleName: ConnectionDrawStyleName;
  

  constructor() {
    this.drawStyle = 'direct';
  }


  ngOnInit() {
  }

}
