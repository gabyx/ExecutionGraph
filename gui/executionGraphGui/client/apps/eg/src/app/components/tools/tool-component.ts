import { IElementEvents } from '@eg/graph';
import { Input, Output, EventEmitter } from '@angular/core';
import { Socket, Node, Graph, Connection } from '../../model';
import { Observable, BehaviorSubject } from 'rxjs';

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
  get enabled(): Observable<boolean> {
    return this._enabled;
  }

  private readonly _enabled = new BehaviorSubject<boolean>(true);

  /**
   * This input can be used to set
   * the enabled state of the tool.
   *
   * @memberof ToolComponent
   */
  @Input()
  set enable(o: boolean) {
    this._enabled.next(o);
  }

  /**
   * This input can be used to set
   * the disabled state ot the tool.
   *
   * @memberof ToolComponent
   */
  @Input()
  set disable(o: boolean) {
    this._enabled.next(!o);
  }

  /**
   * Output Observable which defines if the tool is currently active, e.g. meaning
   * the user is using it.
   * @memberof ToolComponent
   */
  @Output()
  readonly activated = new EventEmitter<boolean>();

  protected activate() {
    this.activated.emit(true);
  }

  protected deactivate() {
    this.activated.emit(false);
  }
}
