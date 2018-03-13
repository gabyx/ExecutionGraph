import { Component, Inject, OnInit, OnChanges, Input, SimpleChanges, SimpleChange, ElementRef } from '@angular/core';

import { GraphComponent } from '../graph/graph.component';
import { Point } from '../../model/Point';

@Component({
  selector: 'ngcs-connection',
  templateUrl: './connection.component.html',
  styleUrls: ['./connection.component.scss']
})
export class ConnectionComponent implements OnInit, OnChanges {
  @Input() from: string;

  @Input() to: string;

  get polyLinePoints(): string {
    const points = this.manhattenLine(this.startPoint, this.endPoint)
    return points.map(p => `${p.x},${p.y}`).join(' ')
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

  private directLine(start: Point, end: Point): Point[] {
    return [start, end];
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
