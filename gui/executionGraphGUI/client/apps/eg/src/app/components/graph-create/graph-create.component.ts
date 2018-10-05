import { Component, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { Observable } from 'rxjs';
import * as fromGraphDescriptions from '../../+state/selectors/'
import { GraphDescriptionsState } from '../../+state/reducers/graphDescription.reducers';
import { GraphTypeDescription } from '../../model';

@Component({
  selector: 'eg-graph-create',
  templateUrl: './graph-create.component.html',
  styleUrls: ['./graph-create.component.css']
})
export class GraphCreateComponent implements OnInit {

  graphTypes: Observable<GraphTypeDescription[]>

  constructor(private store: Store<GraphDescriptionsState>) {
    this.graphTypes = this.store.select(fromGraphDescriptions.getGraphDescriptions);
  }

  ngOnInit() {
  }

}
