import { Component, HostListener, OnInit, EventEmitter } from '@angular/core';
import { Store, select } from '@ngrx/store';
import { Observable } from 'rxjs';
import { withLatestFrom } from 'rxjs/operators';

import { ToolComponent, KEY_CODE } from '../tool-component';
import { GraphsState } from '../../../+state/reducers';
import { Selection } from '../../../+state/reducers/ui.reducers';
import { RemoveNode, ClearSelection } from '../../../+state/actions';
import { getSelection } from '../../../+state/selectors/ui.selectors';
import { Graph } from '../../../model';
import { getSelectedGraph } from '../../../+state/selectors';

@Component({
  selector: 'eg-delete-tool',
  template: ``
})
export class DeleteToolComponent extends ToolComponent implements OnInit {
  private readonly deletePressed = new EventEmitter<void>();
  private readonly selection: Observable<Selection>;
  private readonly selectedGraph: Observable<Graph>;

  constructor(private store: Store<GraphsState>) {
    super();
    this.selection = store.pipe(select(getSelection));
    this.selectedGraph = store.pipe(select(getSelectedGraph));
  }

  ngOnInit() {
    this.deletePressed.pipe(withLatestFrom(this.selection, this.selectedGraph)).subscribe(([_, selection, graph]) => {
      for (const nodeId of selection.nodes) {
        this.store.dispatch(new RemoveNode(graph.id, nodeId));
        this.store.dispatch(new ClearSelection());
      }
    });
  }

  @HostListener('window:keydown', ['$event'])
  onKeyDown(keyEvent: KeyboardEvent) {
    if (keyEvent.key === KEY_CODE.DELETE) {
      keyEvent.preventDefault();
      this.deletePressed.emit();
    }
  }
}
