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
  OnInit,
  ElementRef,
  QueryList,
  HostListener,
  ContentChildren,
  ChangeDetectorRef,
  NgZone,
  AfterViewChecked
} from '@angular/core';
import { PortComponent } from '..//port/port.component';
import { DraggableDirective, DragEvent } from '../../directives/draggable.directive';
import { ConnectionComponent } from '../connection/connection.component';
import { Point } from '../../model/Point';
import { DroppableDirective } from '../../directives/droppable.directive';
import { PositionService } from '@eg/graph/src/lib/services/PositionService';

@Component({
  selector: 'ngcs-graph',
  templateUrl: './graph.component.html',
  styleUrls: ['./graph.component.scss']
})
export class GraphComponent implements OnInit, AfterViewChecked {
  @ContentChildren(PortComponent, { descendants: true })
  ports: QueryList<PortComponent>;

  @ContentChildren(ConnectionComponent, { descendants: true })
  connections: QueryList<ConnectionComponent>;

  public get transformSvg() {
    return `translate(${this.pan.x} ${this.pan.y})`;
  }

  public get transformCss() {
    return `translate(${this.pan.x}px, ${this.pan.y}px)`;
  }

  public get scaleTransform() {
    // return this.sanitizer.bypassSecurityTrustStyle(`scale(${this.zoomFactor})`);
    return `scale(${this.zoomFactor})`;
  }

  public zoomFactor: number = 1;

  public pan: Point = { x: 0, y: 0 };

  private panStart: Point;

  constructor(private element: ElementRef, private cdr: ChangeDetectorRef, private zone: NgZone) {}

  ngOnInit() {
  }

  ngAfterViewChecked() {
    this.cdr.detectChanges();
  }


  /**
   * Handles scrolling for scaling
   * @param e Scroll event
   */
  @HostListener('mousewheel', ['$event'])
  onWindowScroll(e: MouseWheelEvent) {
    e.preventDefault();
    e.cancelBubble = true;
    if (e.wheelDelta < 0) {
      this.zoomFactor *= 0.95;
    } else {
      this.zoomFactor /= 0.95;
    }
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

  /**
   * Handles the start event of the panning (mouse down)
   * @param p Current position
   */
  public onStartPan(p: DragEvent) {
    this.panStart = {
      x: this.pan.x - p.dragElementPosition.x,
      y: this.pan.y - p.dragElementPosition.y
    };
  }

  /**
   * Handles panning events
   * @param p Current position
   */
  public onPan(p: DragEvent) {
    this.pan.x = p.dragElementPosition.x + this.panStart.x;
    this.pan.y = p.dragElementPosition.y + this.panStart.y;
    // console.log(`[WorkspaceComponent] Panning ${this.pan.x}:${this.pan.y}`);
  }

  private getRelativePosition(element: HTMLElement): Point {
    return PositionService.getPositionRelativeToParent(element, this.element.nativeElement);
  }
}
