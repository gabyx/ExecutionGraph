// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jul 29 2018
//  @author Simon Spoerri, simon (døt) spoerri (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import {
  Component,
  ElementRef,
  QueryList,
  HostListener,
  ContentChildren
} from '@angular/core';
import { PortComponent } from '../port/port.component';
import { ConnectionComponent } from '../connection/connection.component';
import { Point, Position } from '../../model/Point';

export function getPositionRelativeToParent(element: HTMLElement, referenceParent?: HTMLElement) {
  let offsetLeft = element.offsetLeft; //+ element.offsetWidth / 2;
  let offsetTop = element.offsetTop; // + element.offsetHeight / 2;

  while (element.offsetParent && element.offsetParent !== referenceParent) {
      element = element.offsetParent as HTMLElement;
      offsetLeft += element.offsetLeft;
      offsetTop += element.offsetTop;
  }

  return {
      x: offsetLeft,
      y: offsetTop
  };
}

@Component({
  selector: 'ngcs-graph',
  templateUrl: './graph.component.html',
  styleUrls: ['./graph.component.scss']
})
export class GraphComponent {
  @ContentChildren(PortComponent, { descendants: true })
  ports: QueryList<PortComponent>;

  @ContentChildren(ConnectionComponent, { descendants: true, })
  connections: QueryList<ConnectionComponent>;

  public get transformSvg() {
    return `translate(${this.pan.x} ${this.pan.y})`;
  }

  public get transformCss() {
    return `translate(${this.pan.x}px, ${this.pan.y}px)`;
  }

  public get scaleTransform() {
    return `scale(${this.zoomFactor})`;
  }

  public zoomFactor: number = 1;

  public pan: Position = { x: 0, y: 0 };

  constructor(private element: ElementRef) {}

  /**
   * Prevent context menu
   * @param e Event
   */
  @HostListener('contextmenu', ['$event'])
  onContextMenu(e: any) {
    e.preventDefault();
  }

  /**
   * Create an SVG Path description for the given connection
   * @param connection Connection to create a path for
   */
  public getPathDescription(connection: ConnectionComponent): string {
    const startPoint = this.getPortPosition(connection.from);
    const endPoint = this.getPortPosition(connection.to);
    // console.log("Updating path");
    let path = connection.drawStyle.getPath(startPoint, endPoint);
    if (typeof path !== 'string') {
      if (path.length < 2) {
        return ``;
      }
      const first = path.shift();
      path = `M${first.x} ${first.y} ${path.map(p => `L${p.x} ${p.y}`).join(' ')}`;
    }
    return path;
  }

  /**
   * Provides the position of the port with the given ID
   * @param id ID of the port
   */
  public getPortPosition(id: string) {
    const port = this.getPort(id);
    if (port) {
      const element: HTMLElement = port.element.nativeElement;

      const position = this.getRelativePosition(element);
      position.x += element.offsetWidth / 2;
      position.y += element.offsetHeight / 2;
      return position;
    }
    return { x: 0, y: 0 };
  }

  /**
   * Provides the port with the given iD
   * @param id ID of the port
   */
  public getPort(id: string): PortComponent {
    if (!this.ports) {
      return null;
    }
    return this.ports.find(p => p.id === id);
  }

  public convertMouseToGraphPosition(mousePoint: Point, offset?: Point) {
    const graphPosition = this.getGraphPosition();
    offset = offset ? offset : {x: 0, y: 0};
    return {
      x: (mousePoint.x - graphPosition.x - offset.x) / this.zoomFactor - this.pan.x,
      y: (mousePoint.y - graphPosition.y - offset.y) / this.zoomFactor - this.pan.y
    };
  }

  public getGraphPosition(): Point {
    return getPositionRelativeToParent(this.element.nativeElement, document.body);
  }

  private getRelativePosition(element: HTMLElement): Point {
    return getPositionRelativeToParent(element, this.element.nativeElement);
  }


}
