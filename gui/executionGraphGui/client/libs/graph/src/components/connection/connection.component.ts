import { Component, Inject, OnInit, OnChanges, Input, SimpleChanges, SimpleChange, ElementRef } from '@angular/core';

import { GraphComponent } from '../graph/graph.component';
import { Point } from '../../model/Point';
import { ConnectionDrawStyle } from '../../model/ConnectionDrawStyle';
import { BezierConnectionDrawStyle } from '../../model/BezierConnectionDrawStyle';

@Component({
  selector: 'ngcs-connection',
  templateUrl: './connection.component.html',
  styleUrls: ['./connection.component.scss']
})
export class ConnectionComponent implements OnInit, OnChanges {
  @Input() from: string;

  @Input() to: string;

  @Input() drawStyle: ConnectionDrawStyle = new BezierConnectionDrawStyle();

  get pathDescription(): string {
    let path = this.drawStyle.getPath(this.startPoint, this.endPoint);
    if (typeof path !== 'string')
    {
      if (path.length<2) {
        return ``;
      }
      const first = path.shift();
      path = `M${first.x} ${first.y} ${path.map(p => `L${p.x} ${p.y}`).join(' ')}`;
    }
    return path;
  }

  get startPoint(): Point {
    return this.graph.getPositionOfChildById(this.from);
  }
  get endPoint(): Point {
    return this.graph.getPositionOfChildById(this.to);
  }

  constructor(@Inject(GraphComponent) private readonly graph: GraphComponent) {}

  ngOnInit() {}

  ngOnChanges(changes: SimpleChanges) {
    if (changes['from'] || changes['to']) {
      this.graph.registerConnection(this.from, this.to);
    }
  }


  private manhattenLine(start: Point, end: Point): Point[] {
    const rangeX = end.x - start.x;
    const rangeY = end.y - start.y;
    if(Math.abs(rangeX) > Math.abs(rangeY)) {
      const midX = start.x + rangeX / 2;
      return [start, {x: midX, y: start.y}, {x: midX, y: end.y}, end];
    }
    else {
      const midY = start.y + rangeY / 2;
      return [start, {x: start.x, y: midY}, {x: end.x, y: midY}, end];
    }
  }
}
