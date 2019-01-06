import { IElementEvents } from '@eg/graph';
import { Input, Output, EventEmitter } from '@angular/core';
import { Socket, Node, Graph, Connection } from '../../model';
import { Observable, BehaviorSubject } from 'rxjs';
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

  /**
   * Output observable which defines the enabled state of the tool.
   * BehaviorSubject because each subscriber should directly get the current value.
   *
   * @type {Observable<boolean>}
   * @memberof ToolComponent
   */
  @Output()
  enabled: Observable<boolean> = new BehaviorSubject<boolean>(true);

  /**
   * This input can be used to set
   * the enabled state ot the tool depending
   * on another observable.
   *
   * @memberof ToolComponent
   */
  @Input()
  set enable(o: Observable<boolean>) {
    this.enabled = o;
  }

  /**
   * This input can be used to set
   * the disabled state ot the tool depending
   * on another observable.
   *
   * @memberof ToolComponent
   */
  @Input()
  set disable(o: Observable<boolean>) {
    this.enabled = o.pipe(map(disable => !disable));
  }

  /**
   * Output Observable which defines if the tool is currently active, e.g. meaning
   * the user is using it.
   * @memberof ToolComponent
   */
  @Output()
  activated = new EventEmitter<boolean>();
}
