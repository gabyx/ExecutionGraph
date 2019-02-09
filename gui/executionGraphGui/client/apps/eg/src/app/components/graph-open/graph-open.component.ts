import { Component, OnInit } from '@angular/core';
import { LoadGraph } from '../../+state/actions';
import { OpenAction } from '../file-browser/file-browser.component';
import { Store } from '@ngrx/store';
import { GraphsState } from '../../+state/reducers';

@Component({
  templateUrl: './graph-open.component.html',
  styleUrls: ['./graph-open.component.scss']
})
export class GraphOpenComponent implements OnInit {
  constructor(private store: Store<GraphsState>) {}

  ngOnInit() {}

  public openGraph(filePath: OpenAction) {
    this.store.dispatch(new LoadGraph(filePath, true));
  }
}
