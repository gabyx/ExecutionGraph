import { Component, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';

import { merge, combineLatest } from 'rxjs';
import { switchMap, tap } from 'rxjs/operators';

import { Point, GraphComponent } from '@eg/graph';
import { ILogger, LoggerFactory } from '@eg/logger';

import { ToolComponent } from '../tool-component';
import { GraphsState } from '../../../+state/reducers';

@Component({
  selector: 'eg-type-tooltip-tool',
  templateUrl: './type-tooltip-tool.component.html',
  styleUrls: ['./type-tooltip-tool.component.scss']
})
export class TypeToolTipToolComponent extends ToolComponent implements OnInit {
  private readonly logger: ILogger;

  private toolTipMessages: string[] = [];
  private toolTipPosition: Point = { x: 0, y: 0 };

  constructor(private graph: GraphComponent, private store: Store<GraphsState>, loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('TypeToolTipToolComponent');
  }


  ngOnInit() {
    const enterEvents = merge(this.socketEvents.onEnter, this.nodeEvents.onEnter);
    const leaveEvents = merge(this.socketEvents.onLeave, this.nodeEvents.onLeave);

    // On entering, (or if enabled changed after enter and before leave)
    combineLatest(enterEvents, this.enabled).pipe(
      tap(([e, enabled]) => {
        if(enabled) {
          this.toolTipMessages = [`Type: [${e.element.type}]`];
          this.toolTipPosition = this.graph.convertMouseToGraphPosition(e.mousePosition);
        }
      }),
      // After it was entereed, only listen to leave again
      switchMap(() => leaveEvents),
      tap(() => {
        this.toolTipMessages = null;
        this.toolTipPosition = { x: 0, y: 0 };
      })
    ).subscribe();
  }
}
