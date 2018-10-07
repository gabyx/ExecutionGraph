import { Component, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { Observable, combineLatest } from 'rxjs';
import { map } from 'rxjs/operators';

import { GraphsState } from '../../+state/reducers';
import { GraphDescriptionsState } from '../../+state/reducers/graphDescription.reducers';
import { NodeTypeDescription } from '../../model';
import { getGraphDescriptionEntities, getSelectedGraph } from '../../+state/selectors';

@Component({
  selector: 'eg-add-node',
  templateUrl: './add-node.component.html',
  styleUrls: ['./add-node.component.css']
})
export class AddNodeComponent implements OnInit {

  public nodeTypes: Observable<NodeTypeDescription[]>;

  constructor(
    private graphStore: Store<GraphsState>,
    private graphDescriptionStore: Store<GraphDescriptionsState>) {

      this.nodeTypes = combineLatest(
        graphDescriptionStore.select(getGraphDescriptionEntities),
        graphStore.select(getSelectedGraph)
      ).pipe(
        map(([descriptions, graph]) => descriptions[graph.typeId.toString()]),
        map(graphType => graphType.nodeTypeDescritptions)
      );

    }

  ngOnInit() {
  }

}
