import { Component, OnInit, Input } from '@angular/core';
import { Connection } from '../../model';
import { ConnectionDrawStyle, BezierConnectionDrawStyle, GraphComponent } from '@eg/graph/src';

@Component({
  selector: 'eg-connection-layer',
  templateUrl: './connection-layer.component.html',
  styleUrls: ['./connection-layer.component.scss']
})
export class ConnectionLayerComponent {

  @Input() connections: Connection[];

  @Input() drawStyle: ConnectionDrawStyle = new BezierConnectionDrawStyle();

  constructor(private graph: GraphComponent) {
  }
  
  public getPathDescription(connection: Connection) {
    const startPoint = this.graph.getPortPosition(connection.inputSocket.idString);
    const endPoint = this.graph.getPortPosition(connection.outputSocket.idString);
    
    
    let path = this.drawStyle.getPath(startPoint, endPoint);
    if (typeof path !== 'string') {
      if (path.length < 2) {
        return ``;
      }
      const first = path.shift();
      path = `M${first.x} ${first.y} ${path.map(p => `L${p.x} ${p.y}`).join(' ')}`;
    }
    return path;
  }

}
