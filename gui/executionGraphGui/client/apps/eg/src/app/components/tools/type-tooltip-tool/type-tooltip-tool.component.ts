import { Component, OnInit, Input } from '@angular/core';
import { Store } from '@ngrx/store';
import { Point, GraphComponent } from '@eg/graph';
import { ILogger, LoggerFactory } from '@eg/logger';

import { ToolComponent } from '../tool-component';
import { GraphsState } from '../../../+state/reducers';
import { Observable, never } from 'rxjs';
import { switchMap } from 'rxjs/operators';
import { IElementEvents } from '@eg/graph';

@Component({
  selector: 'eg-type-tooltip-tool',
  templateUrl: './type-tooltip-tool.component.html',
  styleUrls: ['./type-tooltip-tool.component.scss']
})
export class SocketTypeToolTipToolComponent extends ToolComponent implements OnInit {
  private readonly logger: ILogger;

  private toolTipMessages: string[] = [];
  private toolTipPosition: Point = { x: 0, y: 0 };

  constructor(private graph: GraphComponent, private store: Store<GraphsState>, loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('SocketTypeToolTipToolComponent');
  }

  /**
   * Turns the observable (on/off) depending on the
   * `enabled` observable.
   * @private
   * @template Event
   * @param {Observable<Event>} obs
   * @returns Observable<Event> | never()
   * @memberof SocketTypeToolTipToolComponent
   */
  private makeEnabled<Event>(obs: Observable<Event>) {
    return this.enabled.pipe(
      switchMap(enable => {
        if (enable) {
          return obs;
        } else {
          this.reset();
          return never();
        }
      })
    );
  }

  private subscribe<TElement extends { type?: string }>(events: IElementEvents<TElement>) {
    this.makeEnabled(events.onEnter).subscribe(e => {
      this.toolTipMessages = [`Type: [${e.element.type}]`];
      this.toolTipPosition = this.graph.convertMouseToGraphPosition(e.mousePosition);
    });

    this.makeEnabled(events.onLeave).subscribe(e => this.reset());
  }

  ngOnInit() {
    this.subscribe(this.socketEvents);
    this.subscribe(this.nodeEvents);
  }

  private reset() {
    if (this.toolTipMessages) {
      this.toolTipMessages = null;
      this.toolTipPosition = { x: 0, y: 0 };
    }
  }
}
