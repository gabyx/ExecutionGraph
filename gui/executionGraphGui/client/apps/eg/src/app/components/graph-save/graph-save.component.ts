import { Component, OnInit } from '@angular/core';
import { SaveAction as SaveEvent } from '../file-browser/file-browser.component';
import { getSelectedGraphId } from '../../+state/selectors';
import { select, Store } from '@ngrx/store';
import { Observable, Subject } from 'rxjs';
import { GraphId } from '../../model';
import { GraphsState } from '../../+state/reducers';
import { withLatestFrom } from 'rxjs/operators';
import { SaveGraph } from '../../+state/actions';

@Component({
  templateUrl: './graph-save.component.html',
  styleUrls: ['./graph-save.component.scss']
})
export class GraphSaveComponent implements OnInit {
  private selectedGraphId: Observable<GraphId>;

  private saveSubject = new Subject<SaveEvent>();

  constructor(private store: Store<GraphsState>) {}

  ngOnInit() {
    this.selectedGraphId = this.store.pipe(select(getSelectedGraphId));
    this.saveSubject.pipe(withLatestFrom(this.selectedGraphId)).subscribe(([saveEvent, graphId]) => {
      this.store.dispatch(new SaveGraph(graphId, saveEvent.filePath, saveEvent.overwrite));
    });
  }

  public saveFile(event: SaveEvent) {
    this.saveSubject.next(event);
  }
}
