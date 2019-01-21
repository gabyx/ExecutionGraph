import { Component, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { Observable, combineLatest } from 'rxjs';
import { map, filter } from 'rxjs/operators';

import { GraphsState } from '../../+state/reducers';
import { GraphDescriptionsState } from '../../+state/reducers/graphDescription.reducers';
import { NodeTypeDescription } from '../../model';
import { getGraphDescriptionEntities, getSelectedGraph } from '../../+state/selectors';
import { isDefined } from '@angular/compiler/src/util';

@Component({
  selector: 'eg-add-node',
  templateUrl: './add-node.component.html',
  styleUrls: ['./add-node.component.scss']
})
export class AddNodeComponent implements OnInit {
  public nodeTypes: Observable<NodeTypeDescription[]>;

  constructor(graphStore: Store<GraphsState>, graphDescriptionStore: Store<GraphDescriptionsState>) {
    this.nodeTypes = combineLatest(
      graphDescriptionStore.select(getGraphDescriptionEntities),
      graphStore.select(getSelectedGraph)
    ).pipe(
      filter(([descriptions, graph]) => isDefined(graph)),
      map(([descriptions, graph]) => descriptions[graph.typeId.id()]),
      map(graphType => graphType.nodeTypeDescriptions)
    );
  }

  ngOnInit() {}
}
