import { Component, Input } from '@angular/core';

import { ConnectionDrawStyle } from '../../model/ConnectionDrawStyle';
import { BezierConnectionDrawStyle } from '../../model/BezierConnectionDrawStyle';

@Component({
  selector: 'ngcs-connection',
  templateUrl: './connection.component.html',
  styleUrls: ['./connection.component.scss']
})
export class ConnectionComponent {
  @Input() from: string;

  @Input() to: string;

  @Input() drawStyle: ConnectionDrawStyle = new BezierConnectionDrawStyle();

}
