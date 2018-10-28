import { Component, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { AppState } from '../../+state/reducers/app.reducers';

@Component({
  selector: 'eg-inspector',
  templateUrl: './inspector.component.html',
  styleUrls: ['./inspector.component.scss']
})
export class InspectorComponent implements OnInit {
  constructor(private store: Store<AppState>) {}

  ngOnInit() {}

  public closeInspector() {
  }
}
