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

import { Component, ElementRef, QueryList, HostListener, ContentChildren, ChangeDetectorRef } from '@angular/core';
import { PortComponent } from '../port/port.component';
import { Point, Position } from '../../model/Point';

export function getPositionRelativeToParent(element: HTMLElement, referenceParent?: HTMLElement): Point {
  let offsetLeft = element.offsetLeft; //+ element.offsetWidth / 2;
  let offsetTop = element.offsetTop; // + element.offsetHeight / 2;

  while (element.offsetParent && element.offsetParent !== referenceParent) {
    element = element.offsetParent as HTMLElement;
    offsetLeft += element.offsetLeft;
    offsetTop += element.offsetTop;
  }

  return new Point(offsetLeft, offsetTop);
}

@Component({
  selector: 'ngcs-graph',
  templateUrl: './graph.component.html',
  styleUrls: ['./graph.component.scss']
})
export class GraphComponent {
  // @ContentChildren(PortComponent, { descendants: true })
  // ports: QueryList<PortComponent>;

  // @ContentChildren(ConnectionComponent, { descendants: true, })
  // connections: QueryList<ConnectionComponent>;

  public get transformSvg() {
    return `translate(${this.pan.x} ${this.pan.y})`;
  }

  public get transformCss() {
    return `translate(${this.pan.x}px, ${this.pan.y}px)`;
  }

  public get scaleTransform() {
    return `scale(${this.zoomFactor})`;
  }

  private ports: { [id: string]: PortComponent } = {};

  public zoomFactor: number = 1;

  public pan: Position = Point.one.copy();

  constructor(private element: ElementRef, private cdr: ChangeDetectorRef) {}

  ngAfterViewChecked() {
    this.cdr.detectChanges();
  }

  /**
   * Prevent context menu
   * @param e Event
   */
  @HostListener('contextmenu', ['$event'])
  onContextMenu(e: any) {
    e.preventDefault();
  }

  public registerPort(port: PortComponent) {
    this.ports[port.id] = port;
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
    return Point.one.copy();
  }

  /**
   * Provides the port with the given iD
   * @param id ID of the port
   */
  public getPort(id: string): PortComponent {
    if (!this.ports) {
      return null;
    }
    return this.ports[id];
  }

  public convertMouseToGraphPosition(mousePoint: Point, offset?: Point): Point {
    const graphPosition = this.getGraphPosition();
    offset = offset ? offset : Point.one;
    return new Point(
      (mousePoint.x - graphPosition.x - offset.x) / this.zoomFactor - this.pan.x,
      (mousePoint.y - graphPosition.y - offset.y) / this.zoomFactor - this.pan.y
    );
  }

  public getGraphPosition(): Point {
    return getPositionRelativeToParent(this.element.nativeElement, document.body);
  }

  private getRelativePosition(element: HTMLElement): Point {
    return getPositionRelativeToParent(element, this.element.nativeElement);
  }
}
