import { Component, OnInit } from '@angular/core';
import { Store, select } from '@ngrx/store';
import { Observable } from 'rxjs';
import * as fromGraphDescriptions from '../../+state/selectors/';
import { GraphDescriptionsState } from '../../+state/reducers/graphDescription.reducers';
import { GraphTypeDescription } from '../../model';
import { CreateGraph } from '../../+state/actions';
import { ILogger, LoggerFactory } from '@eg/logger';

@Component({
  selector: 'eg-graph-create',
  templateUrl: './graph-create.component.html',
  styleUrls: ['./graph-create.component.scss']
})
export class GraphCreateComponent implements OnInit {
  graphTypes: Observable<GraphTypeDescription[]>;
  logger: ILogger;

  constructor(private store: Store<GraphDescriptionsState>, readonly loggerFactory: LoggerFactory) {
    this.logger = loggerFactory.create('GraphCreateComponent');
    this.graphTypes = this.store.pipe(select(fromGraphDescriptions.getGraphDescriptions));
  }

  ngOnInit() {}

  public createGraph(graphType: GraphTypeDescription) {
    this.store.dispatch(new CreateGraph(graphType));
  }

  public showDescription(graphType: GraphTypeDescription) {
    this.logger.debug('Show Description');
    //this.router.navigate(['/graph/description', graphType.id.toString()]);
  }
}
