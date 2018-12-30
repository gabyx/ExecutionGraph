import { IElementEvents } from '@eg/graph';
import { Input, Output, EventEmitter } from '@angular/core';
import { Socket, Node, Graph, Connection } from '../../model';
import { Observable, of } from 'rxjs';
import { map, startWith } from 'rxjs/operators';

export enum KEY_CODE {
  BACKSPACE = 8,
  SHIFT = 16,
  CTRL = 17,
  DELETE = 46,
  ESC = 27,
  A = 65
}

export abstract class ToolComponent {
  @Input()
  nodeEvents: IElementEvents<Node>;
  @Input()
  graphEvents: IElementEvents<Graph>;
  @Input()
  socketEvents: IElementEvents<Socket>;
  @Input()
  connectionEvents: IElementEvents<Connection>;

  @Output()
  enabled: Observable<boolean> = of(true);
  @Input()
  set enable(o: Observable<boolean>) {
    this.enabled = o.pipe(startWith(true));
  }

  @Input()
  set disable(o: Observable<boolean>) {
    this.enabled = o.pipe(
      startWith(false),
      map(enable => !enable)
    );
  }

  @Output()
  activated = new EventEmitter<boolean>();
}
