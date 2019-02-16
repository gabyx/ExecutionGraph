import { Component, Input } from '@angular/core';
import { Store, select } from '@ngrx/store';
import { Observable } from 'rxjs';
import { map } from 'rxjs/operators';
import { ConnectionDrawStyle, BezierConnectionDrawStyle, GraphComponent } from '@eg/graph';

import { Connection } from '../../model';
import { UiState } from '../../+state/reducers/ui.reducers';
import { getSelection } from '../../+state/selectors/ui.selectors';

@Component({
  selector: 'eg-connection-layer',
  templateUrl: './connection-layer.component.html',
  styleUrls: ['./connection-layer.component.scss']
})
export class ConnectionLayerComponent {
  @Input() connections: Connection[];

  @Input() drawStyle: ConnectionDrawStyle = new BezierConnectionDrawStyle();

  constructor(private graph: GraphComponent, private store: Store<UiState>) {}

  public getPathDescription(connection: Connection) {
    const startPoint = this.graph.getPortPosition(connection.inputSocket.id);
    const endPoint = this.graph.getPortPosition(connection.outputSocket.id);

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

  public isSelected(connection: Connection): Observable<boolean> {
    return this.store.pipe(
      select(getSelection),
      map(selection => selection.connections.indexOf(connection.id) >= 0)
    );
  }
}
